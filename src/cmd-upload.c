#include "hal.h"
#include "chprintf.h"
#include "spinor.h"
#include "fpga.h"
#include "usbcfg.h"
#include "xmodem.h"

#include "shellcfg.h"

#define driver &SDU1

#define SPIDEV &SPID1

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
  chprintf(chp, "Configuring SPI: ");
  if (spiConfigure(SPIDEV)) {
    chprintf(chp, "Error: Couldn't configure SPI\r\n");
    goto out;
  }
  chprintf(chp, "Ok\r\n");

  chprintf(chp, "Erasing SPINOR: ");
  if (spinorEraseChip(SPIDEV))
    chprintf(chp, "Error"SHELL_NEWLINE_STR);
  else
    chprintf(chp, "Ok"SHELL_NEWLINE_STR);

  int page = 0;
  uint32_t bytes_written = 0;

  chprintf(chp, "Start your uploader now."SHELL_NEWLINE_STR);

  xmodemSendByte(driver, XMODEM_NAK, 200);

  while (!finished) {
    uint8_t sequence_id;
    int ret;

    ret = xmodemReadBlock(driver, spi_data + spi_data_count, sizeof(spi_data), &sequence_id);

    if (ret < 0) {
      retries++;
      xmodemSendByte(driver, XMODEM_NAK, 200);
      continue;
    }
    xmodemSendByte(driver, XMODEM_ACK, 200);

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
  chprintf(chp, "Done."SHELL_NEWLINE_STR);

out:
  /* 4. MCU tri-states its SPI pins */
  chprintf(chp, "Deconfiguring SPI: ");
  if (spiDeconfigure(SPIDEV))
    chprintf(chp, "Error disabling SPI"SHELL_NEWLINE_STR);
  else
    chprintf(chp, "Ok"SHELL_NEWLINE_STR);

  /* 5. MCU re-connects FPGA by asserting FPGA_DRIVE */
  chprintf(chp, "Reconnecting FPGA: ");
  if (fpgaConnect())
    chprintf(chp, "Error connecting FPGA"SHELL_NEWLINE_STR);
  else
    chprintf(chp, "Ok"SHELL_NEWLINE_STR);

  return;
}
