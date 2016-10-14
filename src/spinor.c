#include <string.h>
#include <stdint.h>

#include "hal.h"
#include "spinor.h"

uint8_t spinorGetStatus(SPIDriver *spip)
{
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

void spinorEnableWrite(SPIDriver *spip)
{
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
  while (!(spinorGetStatus(spip) & 0x02))
    chThdSleepMicroseconds(50);
}

void spinorDisableWrite(SPIDriver *spip)
{
  uint8_t byte = 0x04;
  spiSelect(spip);
  spiSend(spip, 1, &byte);
  spiUnselect(spip);

  /* Wait for "Write Enable Latch" to go low */
  while (spinorGetStatus(spip) & 0x02);
}

int msecs;
void spinorEraseChip(SPIDriver *spip)
{
  uint8_t byte = 0xc7;

  spinorEnableWrite(spip);

  spiSelect(spip);
  spiSend(spip, 1, &byte);
  spiUnselect(spip);
  chThdSleepMicroseconds(50);

  /* Wait for "Write In Progress" to go low */
  msecs = 500;
  while (spinorGetStatus(spip) & 0x01) {
    chThdSleepMilliseconds(msecs += 10);
  }
}

void spinorErasePage(SPIDriver *spip, int page)
{

  uint8_t buffer[4];

  // Pages are erased on sector / block boundaries, and there are
  // 16 pages per sector.
  if ((page & 0xf) != 0)
    return;

  buffer[0] = 0x20;
  buffer[1] = page >> 8;
  buffer[2] = page;
  buffer[3] = 0;

  spinorEnableWrite(spip);

  spiSelect(spip);
  spiSend(spip, sizeof(buffer), buffer);
  spiUnselect(spip);
  chThdSleepMicroseconds(50);

  /* Wait for "Write In Progress" to go low */
  while (spinorGetStatus(spip) & 0x01)
    chThdSleepMicroseconds(50);
}

void spinorProgramPage(SPIDriver *spip, int page,
                       size_t count, const void *data)
{
  uint8_t buffer[count + 4];

  buffer[0] = 0x02;
  buffer[1] = page >> 8;
  buffer[2] = page;
  buffer[3] = 0;
  memcpy(buffer + 4, data, count);

  spinorEnableWrite(spip);

  spiSelect(spip);
  spiSend(spip, sizeof(buffer), buffer);
  spiUnselect(spip);
  chThdSleepMicroseconds(50);

  /* Wait for "Write In Progress" to go low */
  while (spinorGetStatus(spip) & 0x01)
    chThdSleepMicroseconds(50);
}