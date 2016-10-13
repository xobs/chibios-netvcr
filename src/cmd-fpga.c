#include <string.h>

#include "hal.h"

#include "fpga.h"

#include "shellcfg.h"
#include "chprintf.h"

void fpgaCommand(BaseSequentialStream *chp, int argc, char *argv[])
{

  if (argc <= 0) {
    chprintf(chp, "Usage: fpga [verb]:"SHELL_NEWLINE_STR);
    chprintf(chp, "    cycle        Power cycle the FPGA"SHELL_NEWLINE_STR);
    chprintf(chp, "    reset        Put the FPGA into reset"SHELL_NEWLINE_STR);
    chprintf(chp, "    unreset      Bring the FPGA out of reset"SHELL_NEWLINE_STR);
    chprintf(chp, "    connect      Connect the FPGA to the SPINOR"SHELL_NEWLINE_STR);
    chprintf(chp, "    disconnect   Disconnect the FPGA to the SPINOR"SHELL_NEWLINE_STR);
    chprintf(chp, "    status       Show status of the FPGA"SHELL_NEWLINE_STR);
    return;
  }

  if (!strcasecmp(argv[0], "cycle")) {
    chprintf(chp, "Power-cycling FPGA: ");

    if (fpgaReset()) {
      chprintf(chp, "Unable to reset"SHELL_NEWLINE_STR);
      return;
    }

    /* Let the FPGA think about things for a while, in reset */
    chThdSleepMilliseconds(1);

    if (fpgaUnreset()) {
      chprintf(chp, "Unable to un-reset"SHELL_NEWLINE_STR);
      return;
    }

    if (fpgaWaitUntilProgrammed(MS2ST(8000))) {
      chprintf(chp, "FPGA never programmed"SHELL_NEWLINE_STR);
      return;
    }

    chprintf(chp, "Ok"SHELL_NEWLINE_STR);
  }

  else if (!strcasecmp(argv[0], "connect")) {
    chprintf(chp, "Connecting FPGA: ");
    if (fpgaConnect())
      chprintf(chp, "Error"SHELL_NEWLINE_STR);
    else
      chprintf(chp, "Ok"SHELL_NEWLINE_STR);
  }

  else if (!strcasecmp(argv[0], "disconnect")) {
    chprintf(chp, "Disconnecting FPGA: ");
    if (fpgaDisconnect())
      chprintf(chp, "Error"SHELL_NEWLINE_STR);
    else
      chprintf(chp, "Ok"SHELL_NEWLINE_STR);
  }

  else if (!strcasecmp(argv[0], "reset")) {
    chprintf(chp, "Putting FPGA into reset: ");
    if (fpgaReset())
      chprintf(chp, "Error"SHELL_NEWLINE_STR);
    else
      chprintf(chp, "Ok"SHELL_NEWLINE_STR);
  }

  else if (!strcasecmp(argv[0], "unreset")) {
    chprintf(chp, "Bringing FPGA out of reset: ");
    if (fpgaUnreset())
      chprintf(chp, "Error"SHELL_NEWLINE_STR);
    else
      chprintf(chp, "Ok"SHELL_NEWLINE_STR);
  }

  else if (!strcasecmp(argv[0], "status")) {
    chprintf(chp, "FPGA programmed: %s"SHELL_NEWLINE_STR,
                                  fpgaProgrammed()?"Yes":"No");
  }

  else {
    chprintf(chp, "Unrecognized command: %s"SHELL_NEWLINE_STR, argv[0]);
  }
}
