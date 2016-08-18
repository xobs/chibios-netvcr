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
#ifndef __RL_USB_H__
#define __RL_USB_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "usb_cdc.h"

/*****************  Functions *************************************************/

/* USB Device functions exported from USB Device Core module                  */
extern void  usbd_init                  (void);
extern void  usbd_connect               (bool con);
extern void  usbd_reset_core            (void);
extern bool  usbd_configured            (void);

/* USB Device user functions imported to USB HID Class module                 */
extern void  usbd_hid_init              (void);
extern bool  usbd_hid_get_report_trigger(uint8_t rid,   uint8_t *buf, int len);
extern int   usbd_hid_get_report        (uint8_t rtype, uint8_t rid, uint8_t *buf, uint8_t  req);
extern void  usbd_hid_set_report        (uint8_t rtype, uint8_t rid, uint8_t *buf, int len, uint8_t req);
extern uint8_t    usbd_hid_get_protocol      (void);
extern void  usbd_hid_set_protocol      (uint8_t protocol);

/* USB Device user functions imported to USB Mass Storage Class module        */
extern void  usbd_msc_init              (void);
extern void  usbd_msc_read_sect         (uint32_t block, uint8_t *buf, uint32_t num_of_blocks);
extern void  usbd_msc_write_sect        (uint32_t block, uint8_t *buf, uint32_t num_of_blocks);
extern void  usbd_msc_start_stop        (bool start);

/* USB Device user functions imported to USB Audio Class module               */
extern void  usbd_adc_init              (void);

/* USB Device CDC ACM class functions called automatically by USBD Core module*/
extern int32_t  USBD_CDC_ACM_Initialize                (void);
extern int32_t  USBD_CDC_ACM_Uninitialize              (void);
extern int32_t  USBD_CDC_ACM_Reset                     (void);
/* USB Device CDC ACM class user functions                                    */
extern int32_t  USBD_CDC_ACM_PortInitialize            (void);
extern int32_t  USBD_CDC_ACM_PortUninitialize          (void);
extern int32_t  USBD_CDC_ACM_PortReset                 (void);
extern int32_t  USBD_CDC_ACM_PortSetLineCoding         (CDC_LINE_CODING *line_coding);
extern int32_t  USBD_CDC_ACM_PortGetLineCoding         (CDC_LINE_CODING *line_coding);
extern int32_t  USBD_CDC_ACM_PortSetControlLineState   (uint16_t ctrl_bmp);
extern int32_t  USBD_CDC_ACM_DataSend                  (const uint8_t *buf, int32_t len);
extern int32_t  USBD_CDC_ACM_DataFree                  (void);
extern int32_t  USBD_CDC_ACM_PutChar                   (const uint8_t  ch);
extern int32_t  USBD_CDC_ACM_DataRead                  (      uint8_t *buf, int32_t len);
extern int32_t  USBD_CDC_ACM_GetChar                   (void);
extern int32_t  USBD_CDC_ACM_DataAvailable             (void);
extern int32_t  USBD_CDC_ACM_Notify                    (uint16_t stat);
/* USB Device CDC ACM class overridable functions                             */
extern int32_t  USBD_CDC_ACM_SendEncapsulatedCommand   (void);
extern int32_t  USBD_CDC_ACM_GetEncapsulatedResponse   (void);
extern int32_t  USBD_CDC_ACM_SetCommFeature            (uint16_t feat);
extern int32_t  USBD_CDC_ACM_GetCommFeature            (uint16_t feat);
extern int32_t  USBD_CDC_ACM_ClearCommFeature          (uint16_t feat);
extern int32_t  USBD_CDC_ACM_SetLineCoding             (void);
extern int32_t  USBD_CDC_ACM_GetLineCoding             (void);
extern int32_t  USBD_CDC_ACM_SetControlLineState       (uint16_t ctrl_bmp);
extern int32_t  USBD_CDC_ACM_SendBreak                 (uint16_t dur);

/* USB Device user functions imported to USB Custom Class module              */
extern void  usbd_cls_init              (void);
extern void  usbd_cls_sof               (void);
extern bool  usbd_cls_dev_req           (bool setup);
extern bool  usbd_cls_if_req            (bool setup);
extern bool  usbd_cls_ep_req            (bool setup);

#ifdef __cplusplus
}
#endif

#endif  /* __RL_USB_H__ */
