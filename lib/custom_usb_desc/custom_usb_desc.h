// #include <usb_desc.h>
// #include <Arduino.h>
#include <stdint.h>

#ifndef USB_DESC_H
#define USB_DESC_H

#define ENDPOINT_UNUSED 0x00
#define ENDPOINT_TRANSMIT_ONLY 0x15
#define ENDPOINT_RECEIVE_ONLY 0x19
#define ENDPOINT_TRANSMIT_AND_RECEIVE 0x1D
#define ENDPOINT_RECEIVE_ISOCHRONOUS 0x18
#define ENDPOINT_TRANSMIT_ISOCHRONOUS 0x14

#define VENDOR_ID 0x16C0
#define PRODUCT_ID 0x0486
#define HID_USAGE_PAGE 0xFF00 // recommended: 0xFF00 to 0xFFFF
#define HID_USAGE 0x0100      // recommended: 0x0100 to 0xFFFF
#define MANUFACTURER_NAME {'T', 'e', 'e', 'n', 's', 'y', 'd', 'u', 'i', 'n', 'o'}
#define MANUFACTURER_NAME_LEN 11
#define PRODUCT_NAME {'C', 'u', 's', 't', 'o', 'm', 'H', 'I', 'D'}
#define PRODUCT_NAME_LEN 9
#define EP0_SIZE 64
#define NUM_ENDPOINTS 2
#define NUM_USB_BUFFERS 12
#define NUM_INTERFACE 1
#define CUSTOM_HID_INTERFACE 0 // RawHID
#define HID_TX_ENDPOINT 1
#define HID_TX_SIZE 64
#define HID_TX_INTERVAL 1
#define HID_RX_ENDPOINT 2
#define HID_RX_SIZE 64
#define HID_RX_INTERVAL 1
#define ENDPOINT1_CONFIG ENDPOINT_TRANSMIT_ONLY
#define ENDPOINT2_CONFIG ENDPOINT_RECEIVE_ONLY

// Device Class = 0x00 (Composite), Interface Class = 0x03 (HID)
#define DEVICE_CLASS 0x00
#define DEVICE_SUBCLASS 0x00
#define DEVICE_PROTOCOL 0x00

// HID Interface Configuration
// #define HID_INTERFACE 0
#define HID_CLASS 0x03 // HID class code
#define HID_SUBCLASS 0x00
#define HID_PROTOCOL 0x00

#ifdef USB_DESC_LIST_DEFINE
#if defined(NUM_ENDPOINTS) && NUM_ENDPOINTS > 0
// NUM_ENDPOINTS = number of non-zero endpoints (0 to 15)
extern const uint8_t usb_endpoint_config_table[NUM_ENDPOINTS];

typedef struct
{
  uint16_t wValue;
  uint16_t wIndex;
  const uint8_t *addr;
  uint16_t length;
} usb_descriptor_list_t;

extern const usb_descriptor_list_t usb_descriptor_list[];
#endif // NUM_ENDPOINTS
#endif // USB_DESC_LIST_DEFINE

#endif
