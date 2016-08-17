#include <stdint.h>
#include <string.h>

#include "hal.h"
#include "spi.h"

#define SPIDEV &SPID1

const uint8_t dumb_rle_file[] =
    #include "fpga_bitstream.h"
;

#define STATE_ZEROES 0
#define STATE_DATA 1

struct dumb_rle_state {
  const uint8_t *data;
  int size;

  const uint8_t *read_offset; // Where in *data we're reading from
  int state;      // 0 = zeroes, 1 = data
  int state_left; // Number of bytes left in this state
  int bytes_left; // Number of bytes left until EOF.
};

static int dumb_rle_get_state(struct dumb_rle_state *state) {
  uint16_t type;

  memcpy(&type, state->read_offset, sizeof(type));

  if (state->bytes_left <= 0)
    return 0;

  state->read_offset += sizeof(type);
  state->bytes_left -= sizeof(type);

  /* If the high bit is set, then it's a run of zeroes. */
  if (type & 0x8000) {
    type &= ~0x8000;
    state->state = STATE_ZEROES;
  }
  else
    state->state = STATE_DATA;
  state->state_left = type;

  return 1;
}

void dumbRleInit(struct dumb_rle_state *state,
                 size_t size,
                 const uint8_t *data)
{

  memset(state, 0, sizeof(*state));
  state->data = data;
  state->size = size;

  state->read_offset = state->data;
  state->bytes_left = state->size;

  dumb_rle_get_state(state);
}

int dumbRleRead(struct dumb_rle_state *state, size_t size, uint8_t *data) {
  int count = 0;

  while (size) {
    /* If there are no bytes left in this state, get a new state */
    if (!state->state_left) {

      /* If we can't get a new state, assume EOF */
      if (!dumb_rle_get_state(state))
        return count;
    }

    if (state->state == STATE_ZEROES) {
      int to_copy = size;
      if (to_copy > state->state_left)
        to_copy = state->state_left;
      memset(data, 0, to_copy);

      state->state_left -= to_copy;
      data += to_copy;
      size -= to_copy;
      count += to_copy;
    }
    else if (state->state == STATE_DATA) {
      int to_copy = size;
      if (to_copy > state->state_left)
        to_copy = state->state_left;

      memcpy(data, state->read_offset, to_copy);
      state->read_offset += to_copy;
      state->bytes_left -= to_copy;
      state->state_left -= to_copy;
      data += to_copy;
      size -= to_copy;
      count += to_copy;
    }
  }

  return count;
}

int dumbRleEof(struct dumb_rle_state *state) {

  if ((state->bytes_left <= 0) && (state->state_left <= 0))
    return 1;
  return 0;
}

int fpgaProgrammed(void) {

  return palReadPad(IOPORT2, 2);
}

void fpgaConnect(void) {
  /* Ensure SPI is disconnected */
  palSetPadMode(IOPORT3, 4, PAL_MODE_INPUT);
  palSetPadMode(IOPORT3, 5, PAL_MODE_INPUT);
  palSetPadMode(IOPORT3, 6, PAL_MODE_INPUT);
  palSetPadMode(IOPORT3, 7, PAL_MODE_INPUT);

  /* Assert FPGA_DRIVE to move the SPI to the FPGA */
  palSetPad(IOPORT2, 0);
  chThdSleepMilliseconds(1);

  /* Now reset the FPGA */

  /* Set FPGA_INIT as an input, to allow it to float */
  palSetPadMode(IOPORT2, 1, PAL_MODE_INPUT);

  /* Put the FPGA into reset */
  palClearPad(IOPORT3, 1); // MCU_F_PROG
  chThdSleepMicroseconds(50);
  palSetPad(IOPORT3, 1); // MCU_F_PROG
}

void fpgaDisconnect(void) {
  /* Deassert FPGA_DRIVE to move the SPI to the MCU */
  palClearPad(IOPORT2, 0);
  chThdSleepMilliseconds(1);
}

static uint8_t spinor_get_status(SPIDriver *spip) {
  uint8_t cmd = 0x05;
  uint8_t val[3];

  spiSelect(spip);
  spiSend(spip, 1, &cmd);
  spiReceive(spip, 1, val);
  spiUnselect(spip);

  /*
  cmd = 0x15;
  spiSelect(spip);
  spiSend(spip, 1, &cmd);
  spiReceive(spip, 2, val + 1);
  spiUnselect(spip);
  */

  return val[0];
}

static void spinor_enable_write(SPIDriver *spip) {
  /*
  uint8_t seq[2] = {0x01, 0x03};
  spiSelect(spip);
  spiSend(spip, 2, seq);
  spiUnselect(spip);
  */

  uint8_t byte = 0x06;
  spiSelect(spip);
  spiSend(spip, 1, &byte);
  spiUnselect(spip);
  chThdSleepMicroseconds(50);

  /* Wait for "Write Enable Latch" to go high */
  while (!(spinor_get_status(spip) & 0x02))
    chThdSleepMicroseconds(50);
}

#if 0
static void spinor_disable_write(SPIDriver *spip) {
  uint8_t byte = 0x04;
  spiSelect(spip);
  spiSend(spip, 1, &byte);
  spiUnselect(spip);

  /* Wait for "Write Enable Latch" to go low */
  while (spinor_get_status(spip) & 0x02);
}
#endif

#if 0
static void spinor_erase_chip(SPIDriver *spip) {
  uint8_t byte = 0x60;

  spiSelect(spip);
  spiSend(spip, 1, &byte);
  spiUnselect(spip);
  chThdSleepMicroseconds(50);

  /* Wait for "Write In Progress" to go low */
  while (spinor_get_status(spip) & 0x01)
    chThdSleepMicroseconds(50);
}
#endif

static void spinor_program_page(SPIDriver *spip, int page,
                             size_t count, const void *data) {
  uint8_t buffer[count + 4];

  buffer[0] = 0x02;
  buffer[1] = page >> 8;
  buffer[2] = page;
  buffer[3] = 0;
  memcpy(buffer + 4, data, count);

  spinor_enable_write(SPIDEV);

  spiSelect(spip);
  spiSend(spip, sizeof(buffer), buffer);
  spiUnselect(spip);
  chThdSleepMicroseconds(50);

  /* Wait for "Write In Progress" to go low */
  while (spinor_get_status(spip) & 0x01)
    chThdSleepMicroseconds(50);
}

static void spinor_erase_page(SPIDriver *spip, int page) {

  uint8_t buffer[4];

  // Pages are erased on sector / block boundaries, and there are
  // 16 pages per sector.
  if ((page & 0xf) != 0)
    return;

  buffer[0] = 0x20;
  buffer[1] = page >> 8;
  buffer[2] = page;
  buffer[3] = 0;

  spinor_enable_write(spip);

  spiSelect(spip);
  spiSend(spip, sizeof(buffer), buffer);
  spiUnselect(spip);
  chThdSleepMicroseconds(50);

  /* Wait for "Write In Progress" to go low */
  while (spinor_get_status(spip) & 0x01)
    chThdSleepMicroseconds(50);
}

static void configure_spi(SPIDriver *spip) {
  static const SPIConfig spinor_config = {
    NULL,
    /* HW dependent part.*/
    GPIOC,
    4,
    KINETIS_SPI_TAR_8BIT_FAST
  };

#warning "Figure out why we need this, and why PCS doesn't work."
  palSetPadMode(IOPORT3, 4, PAL_MODE_OUTPUT_PUSHPULL);
//  palSetPadMode(IOPORT3, 4, PAL_MODE_ALTERNATIVE_2);
  palSetPadMode(IOPORT3, 5, PAL_MODE_ALTERNATIVE_2);
  palSetPadMode(IOPORT3, 6, PAL_MODE_ALTERNATIVE_2);
  palSetPadMode(IOPORT3, 7, PAL_MODE_ALTERNATIVE_2);
  spiStart(spip, &spinor_config);

  /*
   * This seems to be required to get communication to work.
   * Send a dummy byte down the wire, since the first packet is ignored.
   */
#warning "Figure out why this dummy byte is needed"
  spiSelect(spip);
  uint8_t dummy = 0xff;
  spiSend(spip, 1, &dummy);
  spiUnselect(spip);

  spinor_enable_write(spip);
  uint8_t seq[4] = {0x01, 0x00, 0x00, 0x00};
  spiSelect(spip);
  spiSend(spip, 2, seq);
  spiUnselect(spip);
  while (spinor_get_status(spip) & 0x01)
    chThdSleepMilliseconds(1);
}

static void deconfigure_spi(SPIDriver *spip) {
  spiStop(spip);
  palSetPadMode(IOPORT3, 4, PAL_MODE_INPUT);
  palSetPadMode(IOPORT3, 5, PAL_MODE_INPUT);
  palSetPadMode(IOPORT3, 6, PAL_MODE_INPUT);
  palSetPadMode(IOPORT3, 7, PAL_MODE_INPUT);
}

static void read_buffer(SPIDriver *spip)
{
  uint8_t buffer[256];
  uint8_t cmd[4];
  int i;

  for (i = 0x00216c10/256; i < 2192012 / 256; i++) {
    cmd[0] = 0x03;
    cmd[1] = i >> 8;
    cmd[2] = i;
    cmd[3] = 0x00;

    memset(buffer, 0x55, sizeof(buffer));

    spiSelect(spip);
    spiSend(spip, sizeof(cmd), cmd);
    spiReceive(spip, sizeof(buffer), buffer);
    spiUnselect(spip);

    asm("bkpt #0");
  }
}

int bytes_written;
void programDumbRleFile(void) {
  struct dumb_rle_state state;

  while (1) {
    fpgaConnect();

    while (1)
      ;
    chThdSleepSeconds(1);
    asm("bkpt #0");
  }
  /*
    Update SPI ROM:
      1. MCU disconnects FPGA by de-asserting FPGA_DRIVE
      2. MCU configures drive on SPI pins
      3. Data arrives via USB and is written into SPI
      4. MCU tri-states its SPI pins
      5. MCU re-connects FPGA by asserting FPGA_DRIVE
      6. (optional) MCU initiates FPGA configuration by yanking PROGRAM
  */

  dumbRleInit(&state, sizeof(dumb_rle_file), dumb_rle_file);

  /* 1. MCU disconnects FPGA by de-asserting FPGA_DRIVE */
  fpgaDisconnect();

  /* 2. MCU configures drive on SPI pins */
  configure_spi(SPIDEV);
  read_buffer(SPIDEV);

  /* Enable write mode */
  //spinor_enable_write(SPIDEV);
  //spinor_erase_chip(SPIDEV);

  int page = 0;
  bytes_written = 0;
  while (!dumbRleEof(&state)) {
    uint8_t buffer[256];

    int count = dumbRleRead(&state, sizeof(buffer), buffer);
    if (count) {
      spinor_erase_page(SPIDEV, page);
      spinor_program_page(SPIDEV, page++, count, buffer);
      bytes_written += count;
    }
  }

  /* 4. MCU tri-states its SPI pins */
  deconfigure_spi(SPIDEV);

  /* 5. MCU re-connects FPGA by asserting FPGA_DRIVE */
  fpgaConnect();

  asm("bkpt #0");
}
