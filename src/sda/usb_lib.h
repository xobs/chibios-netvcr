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
#ifndef __USB_LIB_H__
#define __USB_LIB_H__

#include <stdint.h>
#include <stdbool.h>

/*------------------------------------------------------------------------------
 *           USB Device Configuration
 *----------------------------------------------------------------------------*/
extern       uint8_t   USBD_AltSetting[];
extern       uint8_t   USBD_EP0Buf[];
extern const uint8_t   usbd_power;
extern const uint8_t   usbd_hs_enable;
extern const uint16_t  usbd_if_num;
extern const uint8_t   usbd_ep_num;
extern const uint8_t   usbd_max_packet0;


/*------------------------------------------------------------------------------
 *           USB Device Class Configuration
 *----------------------------------------------------------------------------*/
extern const uint8_t   usbd_hid_enable;
extern const uint8_t   usbd_hid_if_num;
extern const uint8_t   usbd_hid_ep_intin;
extern const uint8_t   usbd_hid_ep_intout;
extern const uint16_t  usbd_hid_interval     [2];
extern const uint16_t  usbd_hid_maxpacketsize[2];
extern const uint8_t   usbd_hid_inreport_num;
extern const uint8_t   usbd_hid_outreport_num;
extern const uint16_t  usbd_hid_inreport_max_sz;
extern const uint16_t  usbd_hid_outreport_max_sz;
extern const uint16_t  usbd_hid_featreport_max_sz;
extern       uint16_t  USBD_HID_PollingCnt;
extern       uint16_t  USBD_HID_PollingReload[];
extern       uint8_t   USBD_HID_IdleCnt      [];
extern       uint8_t   USBD_HID_IdleReload   [];
extern       uint8_t   USBD_HID_IdleSet      [];
extern       uint8_t   USBD_HID_InReport     [];
extern       uint8_t   USBD_HID_OutReport    [];
extern       uint8_t   USBD_HID_FeatReport   [];

extern const uint8_t   usbd_msc_enable;
extern const uint8_t   usbd_msc_if_num;
extern const uint8_t   usbd_msc_ep_bulkin;
extern const uint8_t   usbd_msc_ep_bulkout;
extern const uint16_t  usbd_msc_maxpacketsize[2];
extern const uint8_t  *usbd_msc_inquiry_data;
extern       uint8_t   USBD_MSC_BulkBuf      [];

extern const uint8_t   usbd_adc_enable;
extern const uint8_t   usbd_adc_cif_num;
extern const uint8_t   usbd_adc_sif1_num;
extern const uint8_t   usbd_adc_sif2_num;
extern const uint8_t   usbd_adc_ep_isoout;
extern const uint32_t  usbd_adc_cfg_datafreq;
extern const uint32_t  usbd_adc_cfg_p_s;
extern const uint32_t  usbd_adc_cfg_p_c;
extern const uint32_t  usbd_adc_cfg_b_s;
extern       int16_t  USBD_ADC_DataBuf      [];

extern const uint8_t   usbd_cdc_acm_enable;
extern const uint8_t   usbd_cdc_acm_cif_num;
extern const uint8_t   usbd_cdc_acm_dif_num;
extern const uint8_t   usbd_cdc_acm_bufsize;
extern const uint8_t   usbd_cdc_acm_ep_intin;
extern const uint8_t   usbd_cdc_acm_ep_bulkin;
extern const uint8_t   usbd_cdc_acm_ep_bulkout;
extern const uint16_t  usbd_cdc_acm_sendbuf_sz;
extern const uint16_t  usbd_cdc_acm_receivebuf_sz;
extern const uint16_t  usbd_cdc_acm_maxpacketsize [2];
extern const uint16_t  usbd_cdc_acm_maxpacketsize1[2];
extern        uint8_t  USBD_CDC_ACM_SendBuf       [];
extern        uint8_t  USBD_CDC_ACM_ReceiveBuf    [];
extern        uint8_t  USBD_CDC_ACM_NotifyBuf     [10];

extern       void usbd_os_evt_set       (uint16_t event_flags, uint32_t task);
extern       uint16_t  usbd_os_evt_get       (void);
extern       uint32_t  usbd_os_evt_wait_or   (uint16_t wait_flags, uint16_t timeout);

extern const bool __rtx;


/*------------------------------------------------------------------------------
 *      USB Device Descriptors
 *----------------------------------------------------------------------------*/
extern const uint8_t   USBD_HID_ReportDescriptor[];
extern const uint16_t  USBD_HID_ReportDescriptorSize;
extern const uint16_t  USBD_HID_DescriptorOffset;
extern const uint8_t   USBD_DeviceDescriptor[];
extern const uint8_t   USBD_DeviceQualifier[];
extern const uint8_t   USBD_DeviceQualifier_HS[];
extern const uint8_t   USBD_ConfigDescriptor[];
extern const uint8_t   USBD_ConfigDescriptor_HS[];
extern const uint8_t   USBD_OtherSpeedConfigDescriptor[];
extern const uint8_t   USBD_OtherSpeedConfigDescriptor_HS[];
extern const uint8_t   USBD_OtherSpeedConfigDescriptor[];
extern const uint8_t   USBD_OtherSpeedConfigDescriptor_HS[];
extern const uint8_t   USBD_StringDescriptor[];

#endif  /* __USB_LIB_H__ */
