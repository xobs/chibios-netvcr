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
#ifndef __USBD_CORE_H__
#define __USBD_CORE_H__

#include <stdint.h>
#include "usb_def.h"

/*--------------------------- Data structures --------------------------------*/

/* USB Device Core Endpoint Data Structure */
typedef struct _USBD_EP_DATA {
  uint8_t *pData;
  uint16_t Count;
} USBD_EP_DATA;


/*--------------------------- Global variables -------------------------------*/

/* USB Device Core Global Variables */
extern uint16_t    USBD_DeviceStatus;
extern uint8_t     USBD_DeviceAddress;
extern uint8_t     USBD_Configuration;
extern uint32_t    USBD_EndPointMask;
extern uint32_t    USBD_EndPointHalt;
extern uint32_t    USBD_EndPointStall;
extern uint8_t     USBD_NumInterfaces;
extern uint8_t     USBD_HighSpeed;
extern uint8_t     USBD_ZLP;

extern USBD_EP_DATA     USBD_EP0Data;
extern USB_SETUP_PACKET USBD_SetupPacket;

extern void *      USBD_RTX_DevTask;
extern void *      USBD_RTX_EPTask[];
extern void *      USBD_RTX_CoreTask;


/*--------------------------- Functions exported to class specific files -----*/

extern void        USBD_SetupStage     (void);
extern void        USBD_DataInStage    (void);
extern void        USBD_DataOutStage   (void);
extern void        USBD_StatusInStage  (void);
extern void        USBD_StatusOutStage (void);


/*--------------------------- Event handling routines ------------------------*/

extern        void usbd_class_init    (void);

extern        void USBD_EndPoint0     (uint32_t event);

extern        void USBD_RTX_EndPoint0 (void);


#endif  /* __USBD_CORE_H__ */
