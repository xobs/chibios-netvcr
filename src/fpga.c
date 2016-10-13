#include "hal.h"

#define FPGA_TIMEOUT MS2ST(500)

int fpgaProgrammed(void)
{

  return palReadPad(IOPORT3, 2);
}

int fpgaConnect(void)
{
  /* Ensure SPI is disconnected */
  palSetPadMode(IOPORT3, 4, PAL_MODE_INPUT);
  palSetPadMode(IOPORT3, 5, PAL_MODE_INPUT);
  palSetPadMode(IOPORT3, 6, PAL_MODE_INPUT);
  palSetPadMode(IOPORT3, 7, PAL_MODE_INPUT);

  /* Let FPGA_DRIVE float up, to move the SPI to the FPGA */
  palSetPadMode(IOPORT2, 0, PAL_MODE_INPUT);
  chThdSleepMilliseconds(1);

  /* Set FPGA_INIT as an input, to allow it to float */
  palSetPadMode(IOPORT2, 1, PAL_MODE_INPUT);

  /* Reset (then un-reset) the FPGA */
  palSetPadMode(IOPORT3, 1, PAL_MODE_OUTPUT_PUSHPULL);
  palClearPad(IOPORT3, 1); // MCU_F_PROG
  chThdSleepMicroseconds(50);
  palSetPad(IOPORT3, 1); // MCU_F_PROG

  /* Wait for it to program itself */
  int start_time = chVTGetSystemTimeX();
  int sleep_msecs = 1;
  while (!fpgaProgrammed()) {
    if (chVTTimeElapsedSinceX(start_time) > FPGA_TIMEOUT)
      return MSG_TIMEOUT;
    chThdSleepMilliseconds(sleep_msecs += 5);
  }

  palClearPad(IOPORT1, 4); // LED on
  return 0;
}

int fpgaDisconnect(void)
{
  /* Deassert FPGA_DRIVE to move the SPI to the MCU */
  palSetPadMode(IOPORT2, 0, PAL_MODE_OUTPUT_PUSHPULL);
  palClearPad(IOPORT2, 0);
  chThdSleepMilliseconds(1);

  return 0;
}
