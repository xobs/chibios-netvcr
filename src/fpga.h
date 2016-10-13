#ifndef __FPGA_H__
#define __FPGA_H__

int fpgaProgrammed(void);
int fpgaConnect(void);
int fpgaDisconnect(void);
int fpgaReset(void);
int fpgaUnreset(void);
int fpgaWaitUntilProgrammed(uint32_t max_ticks);

#endif /* __FPGA_H__ */
