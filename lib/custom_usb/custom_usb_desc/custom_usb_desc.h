// #include <usb_desc.h>
// #include <Arduino.h>

#ifndef USB_DESC_H
#define USB_DESC_H

// Vendor ID (Teensy's official VID)
#define VENDOR_ID           0x16C0

// Change Product ID to non-Serial value
#define PRODUCT_ID          0x0486  // <-- Different from 0x0483 (Teensyduino Serial)

// Device Class = 0x00 (Composite), Interface Class = 0x03 (HID)
#define DEVICE_CLASS        0x00
#define DEVICE_SUBCLASS     0x00
#define DEVICE_PROTOCOL     0x00

// HID Interface Configuration
#define HID_INTERFACE       0
#define HID_CLASS           0x03    // HID class code
#define HID_SUBCLASS        0x00
#define HID_PROTOCOL        0x00

// Endpoint Settings
#define HID_TX_ENDPOINT     3
#define HID_RX_ENDPOINT     4
#define HID_TX_SIZE         64
#define HID_RX_SIZE         64
#define EP0_SIZE            64

// ----------------------------
// HID Report Descriptor
// ----------------------------
#define CUSTOM_HID_REPORT_DESC \
  0x06, 0x00, 0xFF,  /* Usage Page (Vendor Defined 0xFF00) */ \
  0x09, 0x01,        /* Usage 0x01 (Potentiometer) */ \
  0xA1, 0x01,        /* Collection (Application) */ \
  \
  /* 4x 16-bit Pots */ \
  0x09, 0x01,        /* Usage 0x01 (Pot) */ \
  0x15, 0x00,        /* Logical Minimum (0) */ \
  0x26, 0xFF, 0xFF,  /* Logical Maximum (65535) */ \
  0x75, 0x10,        /* Report Size (16 bits) */ \
  0x95, 0x04,        /* Report Count (4) */ \
  0x81, 0x02,        /* Input (Data, Variable, Absolute) */ \
  \
  /* 8x 1-bit Switches */ \
  0x05, 0xFF,        /* Usage Page (Vendor Defined) */ \
  0x09, 0x02,        /* Usage 0x02 (Switch) */ \
  0x15, 0x00,        /* Logical Minimum (0) */ \
  0x25, 0x01,        /* Logical Maximum (1) */ \
  0x75, 0x01,        /* Report Size (1 bit) */ \
  0x95, 0x08,        /* Report Count (8) */ \
  0x81, 0x02,        /* Input (Data, Variable, Absolute) */ \
  \
  0xC0               /* End Collection */

// ----------------------------
// HID Class Descriptor
// ----------------------------
#define HID_DESCRIPTOR \
  0x09,        /* bLength: HID Descriptor size */ \
  0x21,        /* bDescriptorType: HID */ \
  0x11, 0x01,  /* bcdHID: HID Class Spec 1.11 */ \
  0x00,        /* bCountryCode: Not localized */ \
  0x01,        /* bNumDescriptors: 1 report */ \
  0x22,        /* bDescriptorType: Report */ \
  sizeof(CUSTOM_HID_REPORT_DESC), 0x00, /* wReportLength */ \

// ----------------------------
// USB Configuration Descriptor
// ----------------------------
#define NUM_INTERFACE       1
#define NUM_ENDPOINTS       2
#define CONFIG_DESC_SIZE    (9 + 9 + 9 + 7 + 7)

#define ENDPOINT1_CONFIG    ENDPOINT_TRANSMIT_ONLY
#define ENDPOINT2_CONFIG    ENDPOINT_RECEIVE_ONLY

#endif // USB_DESC_H

