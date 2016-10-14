#ifndef __SPINOR_H__
#define __SPINOR_H__

struct SPIDriver;

uint8_t spinorGetStatus(SPIDriver *spip);
void spinorEnableWrite(SPIDriver *spip);
void spinorDisableWrite(SPIDriver *spip);
void spinorEraseChip(SPIDriver *spip);
void spinorErasePage(SPIDriver *spip, int page);
void spinorProgramPage(SPIDriver *spip, int page,
                       size_t count, const void *data);

#endif /* __SPINOR_H__ */