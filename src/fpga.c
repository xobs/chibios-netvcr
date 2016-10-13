#include "hal.h"
#include "fpga.h"

int fpgaProgrammed(void)
{

  return palReadPad(IOPORT3, 2);
}

int fpgaWaitUntilProgrammed(uint32_t max_ticks)
{
  /* Wait for it to program itself */
  int start_time = chVTGetSystemTimeX();
  while (!fpgaProgrammed()) {
    if (chVTTimeElapsedSinceX(start_time) > max_ticks)
      return MSG_TIMEOUT;
  }
  return 0;
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

  /* Set FPGA_INIT as an input, to allow it to float */
  palSetPadMode(IOPORT2, 1, PAL_MODE_INPUT);

  /* Let things float */
  chThdSleepMilliseconds(1);

  /* Un-reset the FPGA */
  fpgaUnreset();

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

int fpgaReset(void)
{
  palClearPad(IOPORT3, 1); // MCU_F_PROG
  palSetPadMode(IOPORT3, 1, PAL_MODE_OUTPUT_PUSHPULL);
  return 0;
}

int fpgaUnreset(void)
{
  palSetPad(IOPORT3, 1); // MCU_F_PROG
  palSetPadMode(IOPORT3, 1, PAL_MODE_OUTPUT_PUSHPULL);
  return 0;
}
