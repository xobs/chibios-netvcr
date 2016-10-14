#include "hal.h"
#include "chprintf.h"
#include "spinor.h"
#include "fpga.h"
#include "usbcfg.h"

extern void spiConfigure(SPIDriver *spip);
extern void spiDeconfigure(SPIDriver *spip);

#define driver &SDU1

#define SPIDEV &SPID1

#define XMODEM_STX 0x02
#define XMODEM_SOH 0x01
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define YMODEM_CAN 0x18
#define YMODEM_EOT 0x04
#define XMODEM_C   0x43

static int receive_data(SerialUSBDriver *chp, void *data,
                        uint32_t size, uint32_t timeout_ms)
{
  return chnReadTimeout(chp, data, size, MS2ST(timeout_ms));
}

static int send_byte(SerialUSBDriver *chp, uint8_t byte, uint32_t timeout_ms)
{
  return chnWriteTimeout(chp, &byte, 1, timeout_ms);
}

int read_block(SerialUSBDriver *chp, void *data, uint32_t max_data,
               uint8_t *sequence_id)
{
  int ret;
  uint8_t header;
  int data_size;
  (void)max_data;

  /* Read block header */
  ret = receive_data(chp, &header, 1, 1000);
  if (ret != 1)
    return -1;

  switch(header) {
  case XMODEM_STX:
    data_size = 1024;
    break;

  case XMODEM_SOH:
    data_size = 128;
    break;

  case YMODEM_EOT:
    return 0;
    break;

  case YMODEM_CAN:
    return 0;
    break;

  default:
    return -1;
  }

  // Bytes 2 and 3 of the packet indicate the 8-bit sequence number.
  int8_t block_number[2] = {};
  ret = receive_data(chp, block_number, sizeof(block_number), 1000);
  if (ret != sizeof(block_number))
    return -1;

  if (sequence_id)
    *sequence_id = block_number[0];

  if (block_number[1] != ~block_number[0])
    return -1;

  /* Now actually receive the packet */
  ret = receive_data(chp, data, data_size, 500);
  if (ret < 0)
    return ret;

  /* Do a cheesy recalculation of the checksum */
  int i;
  uint8_t checksum_calc = 0;
  uint8_t checksum_val = 0;

  for (i = 0; i < ret; i++)
    checksum_calc += ((uint8_t *)data)[i];

  ret = receive_data(chp, &checksum_val, sizeof(checksum_val), 500);
  if (ret < 0)
    return ret;

  if (checksum_calc != checksum_val)
    return -1;

  return data_size;
}

__attribute__((aligned(4))) static uint8_t spi_data[256];
static uint32_t spi_data_count;

void handleUpload(BaseSequentialStream *chp, int argc, char *argv[])
{
  int finished = 0;
  int retries = 0;
  uint8_t last_sequence_id = 0;
  (void)argc;
  (void)argv;

  /*
    Update SPI ROM:
      1. MCU disconnects FPGA by de-asserting FPGA_DRIVE
      2. MCU configures drive on SPI pins
      3. Data arrives via USB and is written into SPI
      4. MCU tri-states its SPI pins
      5. MCU re-connects FPGA by asserting FPGA_DRIVE
      6. (optional) MCU initiates FPGA configuration by yanking PROGRAM
  */

  /* 1. MCU disconnects FPGA by de-asserting FPGA_DRIVE */
  chprintf(chp, "Disconnecting FPGA...\r\n");
  fpgaDisconnect();

  /* 2. MCU configures drive on SPI pins */
  chprintf(chp, "Configuring SPI...\r\n");
  spiConfigure(SPIDEV);

  chprintf(chp, "Erasing SPINOR...\r\n");
  spinorEraseChip(SPIDEV);

  int page = 0;
  uint32_t bytes_written = 0;


  chprintf(chp, "Start your uploader now.\r\n");

  send_byte(driver, XMODEM_NAK, 200);

  while (!finished) {
    uint8_t sequence_id;
    int ret;

    ret = read_block(driver, spi_data + spi_data_count, sizeof(spi_data), &sequence_id);

    if (ret < 0) {
      retries++;
      send_byte(driver, XMODEM_NAK, 200);
      continue;
   }
    send_byte(driver, XMODEM_ACK, 200);

   /* Pad out the final packet with 0xff */
   if (ret == 0) {
     while (spi_data_count <= sizeof(spi_data))
      spi_data[spi_data_count++] = 0xff;
      finished = 1;
   }

    if (retries >= 500)
      finished = 1;

    if (sequence_id == last_sequence_id) {
      continue;
   }

    last_sequence_id = sequence_id;

    spi_data_count += ret;
    if (spi_data_count >= sizeof(spi_data)) {
      spinorProgramPage(SPIDEV, page++, spi_data_count, spi_data);
      bytes_written += spi_data_count;
      spi_data_count = 0;
    }
  }

  /* 4. MCU tri-states its SPI pins */
  chprintf(chp, "Deconfiguring SPI...\r\n");
  spiDeconfigure(SPIDEV);

  /* 5. MCU re-connects FPGA by asserting FPGA_DRIVE */
  chprintf(chp, "Reconnecting FPGA...\r\n");
  fpgaConnect();

  chprintf(chp, "Done.\r\n");
  return;
}