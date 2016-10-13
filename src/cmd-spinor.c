#include "hal.h"

#include "spinor.h"

#include "shellcfg.h"
#include "chprintf.h"

#define SPIDEV &SPID1

void spinorCommand(BaseSequentialStream *chp, int argc, char *argv[])
{

  chprintf(chp, "Starting spi: ");
  if (spiConfigure(SPIDEV))
    chprintf(chp, "Error"SHELL_NEWLINE_STR);
  else
    chprintf(chp, "Ok"SHELL_NEWLINE_STR);

  chprintf(chp, "Status: 0x%02x"SHELL_NEWLINE_STR, spinorGetStatus(SPIDEV));
  chprintf(chp, "Id: 0x%04x"SHELL_NEWLINE_STR, spinorReadDeviceId(SPIDEV));
  chprintf(chp, "Signature: 0x%04x"SHELL_NEWLINE_STR, spinorReadElectronicSignature(SPIDEV));

  int i;
  for (i = 0; i < 256; i++) {
    chprintf(chp, "  Id %d: 0x%04x"SHELL_NEWLINE_STR, i, spinorReadDeviceIdType(SPIDEV, i));
  }

  if (spiDeconfigure(SPIDEV))
    chprintf(chp, "Error"SHELL_NEWLINE_STR);
  else
    chprintf(chp, "Ok"SHELL_NEWLINE_STR);
}

