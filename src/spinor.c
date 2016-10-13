#include <string.h>
#include <stdint.h>

#include "hal.h"
#include "spinor.h"

uint8_t status_logs[16];
static uint8_t status_log_ptr;

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

  status_logs[status_log_ptr++] = val[0];
  status_log_ptr &= 0xf;

  return val[0];
}

uint32_t spinorReadDeviceId(SPIDriver *spip)
{

  uint8_t cmd = 0xab;
  uint32_t val;

  spiSelect(spip);
  spiSend(spip, 1, &cmd);
  spiReceive(spip, 4, &val);
  spiUnselect(spip);
  return val;
}

uint32_t spinorReadElectronicSignature(SPIDriver *spip)
{

  uint8_t cmd[] = {0xab, 0x00, 0x00, 0x00};
  uint32_t val;

  spiSelect(spip);
  spiSend(spip, sizeof(cmd), cmd);
  spiReceive(spip, 4, &val);
  spiUnselect(spip);
  return val;
}

uint32_t spinorReadDeviceIdType(SPIDriver *spip, uint8_t id)
{

  uint8_t cmd[] = {0x90, 0x00, 0x00, id};
  uint32_t val;

  spiSelect(spip);
  spiSend(spip, 1, &cmd);
  spiReceive(spip, 4, &val);
  spiUnselect(spip);
  return val;
}

int spinorEnableWrite(SPIDriver *spip)
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

  return 0;
}

int spinorDisableWrite(SPIDriver *spip)
{
  uint8_t byte = 0x04;
  spiSelect(spip);
  spiSend(spip, 1, &byte);
  spiUnselect(spip);

  /* Wait for "Write Enable Latch" to go low */
  while (spinorGetStatus(spip) & 0x02)
    ;

  return 0;
}

int spinorEraseChip(SPIDriver *spip)
{
  uint8_t byte = 0xc7;

  spinorEnableWrite(spip);

  spiSelect(spip);
  spiSend(spip, 1, &byte);
  spiUnselect(spip);
  chThdSleepMicroseconds(50);

  /* Wait for "Write In Progress" to go low */
  int msecs = 500;
  while (spinorGetStatus(spip) & 0x01) {
    chThdSleepMilliseconds(msecs += 10);
  }

  return 0;
}

int spinorErasePage(SPIDriver *spip, int page)
{

  uint8_t buffer[4];

  // Pages are erased on sector / block boundaries, and there are
  // 16 pages per sector.
  if ((page & 0xf) != 0)
    return -1;

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

  return 0;
}

int spinorProgramPage(SPIDriver *spip, int page,
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

  return 0;
}
