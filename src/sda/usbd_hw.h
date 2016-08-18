/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2009-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __USBD_HW_H__
#define __USBD_HW_H__

#include <stdint.h>
#include <stdbool.h>
#include "usb_def.h"

/* USB Hardware Functions */
extern void USBD_Init        (void);
extern void USBD_Connect     (bool con);
extern void USBD_Reset       (void);
extern void USBD_Suspend     (void);
extern void USBD_Resume      (void);
extern void USBD_WakeUp      (void);
extern void USBD_WakeUpCfg   (bool cfg);
extern void USBD_SetAddress  (uint32_t  adr, uint32_t setup);
extern void USBD_Configure   (bool cfg);
extern void USBD_ConfigEP    (USB_ENDPOINT_DESCRIPTOR *pEPD);
extern void USBD_DirCtrlEP   (uint32_t  dir);
extern void USBD_EnableEP    (uint32_t  EPNum);
extern void USBD_DisableEP   (uint32_t  EPNum);
extern void USBD_ResetEP     (uint32_t  EPNum);
extern void USBD_SetStallEP  (uint32_t  EPNum);
extern void USBD_ClrStallEP  (uint32_t  EPNum);
extern void USBD_ClearEPBuf  (uint32_t  EPNum);
extern uint32_t  USBD_ReadEP      (uint32_t  EPNum, uint8_t *pData);
extern uint32_t  USBD_WriteEP     (uint32_t  EPNum, uint8_t *pData, uint32_t cnt);
extern uint32_t  USBD_GetFrame    (void);
extern uint32_t  USBD_GetError    (void);

#endif  /* __USBD_HW_H__ */
