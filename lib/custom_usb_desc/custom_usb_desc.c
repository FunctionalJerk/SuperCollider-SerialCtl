/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if F_CPU >= 20000000

#define USB_DESC_LIST_DEFINE
#include "custom_usb_desc.h"
#ifdef NUM_ENDPOINTS
#include "custom_usb_names.h"
#include <stddef.h>
// #include "kinetis.h"
// #include "avr_functions.h"

// USB Descriptors are binary data which the USB host reads to
// automatically detect a USB device's capabilities.  The format
// and meaning of every field is documented in numerous USB
// standards.  When working with USB descriptors, despite the
// complexity of the standards and poor writing quality in many
// of those documents, remember descriptors are nothing more
// than constant binary data that tells the USB host what the
// device can do.  Computers will load drivers based on this data.
// Those drivers then communicate on the endpoints specified by
// the descriptors.

// To configure a new combination of interfaces or make minor
// changes to existing configuration (eg, change the name or ID
// numbers), usually you would edit "usb_desc.h".  This file
// is meant to be configured by the header, so generally it is
// only edited to add completely new USB interfaces or features.



// **************************************************************
//   USB Device
// **************************************************************

#define LSB(n) ((n) & 255)
#define MSB(n) (((n) >> 8) & 255)

// USB Device Descriptor.  The USB host reads this first, to learn
// what type of device is connected.
static uint8_t device_descriptor[] = {
        18,                                     // bLength
        1,                                      // bDescriptorType
        0x10, 0x01,                             // bcdUSB
#ifdef DEVICE_CLASS
        DEVICE_CLASS,                           // bDeviceClass
#else
	0,
#endif
#ifdef DEVICE_SUBCLASS
        DEVICE_SUBCLASS,                        // bDeviceSubClass
#else
	0,
#endif
#ifdef DEVICE_PROTOCOL
        DEVICE_PROTOCOL,                        // bDeviceProtocol
#else
	0,
#endif
        EP0_SIZE,                               // bMaxPacketSize0
        LSB(VENDOR_ID), MSB(VENDOR_ID),         // idVendor
        LSB(PRODUCT_ID), MSB(PRODUCT_ID),       // idProduct
#ifdef BCD_DEVICE
	LSB(BCD_DEVICE), MSB(BCD_DEVICE),       // bcdDevice
#else
  // For USB types that don't explicitly define BCD_DEVICE,
  // use the minor version number to help teensy_ports
  // identify which Teensy model is used.
  #if defined(__MKL26Z64__)
        0x73, 0x02,
  #elif defined(__MK20DX128__)
        0x74, 0x02,
  #elif defined(__MK20DX256__)
        0x75, 0x02,
  #elif defined(__MK64FX512__)
        0x76, 0x02,
  #elif defined(__MK66FX1M0__)
        0x77, 0x02,
  #else
        0x00, 0x02,
  #endif
#endif
        1,                                      // iManufacturer
        2,                                      // iProduct
        3,                                      // iSerialNumber
        1                                       // bNumConfigurations
};

// These descriptors must NOT be "const", because the USB DMA
// has trouble accessing flash memory with enough bandwidth
// while the processor is executing from flash.



// **************************************************************
//   HID Report Descriptors
// **************************************************************

// Each HID interface needs a special report descriptor that tells
// the meaning and format of the data.

#ifdef CUSTOM_HID_INTERFACE 
static uint8_t customhid_report_desc[] = {
  0x06, 0x00, 0xFF,  /* Usage Page (Vendor Defined 0xFF00) */ 
  0x09, 0x01,        /* Usage 0x01 (Potentiometer) */ 
  0xA1, 0x01,        /* Collection (Application) */ 
  
  /* 4x 16-bit Pots */ \
  0x09, 0x01,        /* Usage 0x01 (Pot) */ 
  0x15, 0x00,        /* Logical Minimum (0) */ 
  0x26, 0xFF, 0xFF,  /* Logical Maximum (65535) */ 
  0x75, 0x10,        /* Report Size (16 bits) */ 
  0x95, 0x04,        /* Report Count (4) */ 
  0x81, 0x02,        /* Input (Data, Variable, Absolute) */ 
  
  /* 8x 1-bit Switches */ 
  0x05, 0xFF,        /* Usage Page (Vendor Defined) */ 
  0x09, 0x02,        /* Usage 0x02 (Switch) */ 
  0x15, 0x00,        /* Logical Minimum (0) */ 
  0x25, 0x01,        /* Logical Maximum (1) */ 
  0x75, 0x01,        /* Report Size (1 bit) */ 
  0x95, 0x08,        /* Report Count (8) */ 
  0x81, 0x02,        /* Input (Data, Variable, Absolute) */ 
  0xC0               /* End Collection */
};
#endif

#define CONFIG_HEADER_DESCRIPTOR_SIZE	9
#define CUSTOM_HID_INTERFACE_DESC_POS   CONFIG_HEADER_DESCRIPTOR_SIZE
#ifdef  CUSTOM_HID_INTERFACE
#define HID_INTERFACE_DESC_SIZE	        9+9+7+7
#define HID_DESC_OFFSET		        CUSTOM_HID_INTERFACE_DESC_POS+9
#else
#define RAWHID_INTERFACE_DESC_SIZE	0
#endif
#define CONFIG_DESC_SIZE

// **************************************************************
//   USB Configuration
// **************************************************************

// USB Configuration Descriptor.  This huge descriptor tells all
// of the devices capabilities.
static uint8_t config_descriptor[CONFIG_DESC_SIZE] = {
        // configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
        9,                                      // bLength;
        2,                                      // bDescriptorType;
        LSB(CONFIG_DESC_SIZE),                 // wTotalLength
        MSB(CONFIG_DESC_SIZE),
        NUM_INTERFACE,                          // bNumInterfaces
        1,                                      // bConfigurationValue
        0,                                      // iConfiguration
        0xC0,                                   // bmAttributes
        50,                                     // bMaxPower

#ifdef RAWHID_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        RAWHID_INTERFACE,                       // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(rawhid_report_desc)),        // wDescriptorLength
        MSB(sizeof(rawhid_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        RAWHID_TX_ENDPOINT | 0x80,              // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        RAWHID_TX_SIZE, 0,                      // wMaxPacketSize
        RAWHID_TX_INTERVAL,                     // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        RAWHID_RX_ENDPOINT,                     // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        RAWHID_RX_SIZE, 0,                      // wMaxPacketSize
        RAWHID_RX_INTERVAL,			// bInterval
#endif // RAWHID_INTERFACE



#ifdef CUSTOM_HID_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        CUSTOM_HID_INTERFACE,                       // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(customhid_report_desc)),        // wDescriptorLength
        MSB(sizeof(customhid_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        HID_TX_ENDPOINT | 0x80,              // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        HID_TX_SIZE, 0,                      // wMaxPacketSize
        HID_TX_INTERVAL,                     // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        HID_RX_ENDPOINT,                     // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        HID_RX_SIZE, 0,                      // wMaxPacketSize
        HID_RX_INTERVAL,			// bInterval
#endif // RAWHID_INTERFACE

};


// **************************************************************
//   String Descriptors
// **************************************************************

// The descriptors above can provide human readable strings,
// referenced by index numbers.  These descriptors are the
// actual string data

/* defined in usb_names.h
struct usb_string_descriptor_struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t wString[];
};
*/

extern struct usb_string_descriptor_struct usb_string_manufacturer_name
        __attribute__ ((weak, alias("usb_string_manufacturer_name_default")));
extern struct usb_string_descriptor_struct usb_string_product_name
        __attribute__ ((weak, alias("usb_string_product_name_default")));
extern struct usb_string_descriptor_struct usb_string_serial_number
        __attribute__ ((weak, alias("usb_string_serial_number_default")));

#ifdef MIDI_INTERFACE
        extern struct usb_string_descriptor_struct usb_string_midi_port1
                __attribute__ ((weak, alias("usb_string_midi_port1_default")));
#if MIDI_NUM_CABLES >= 2
        extern struct usb_string_descriptor_struct usb_string_midi_port2
                __attribute__ ((weak, alias("usb_string_midi_port2_default")));
#endif
#if MIDI_NUM_CABLES >= 3
        extern struct usb_string_descriptor_struct usb_string_midi_port3
                __attribute__ ((weak, alias("usb_string_midi_port3_default")));
#endif
#if MIDI_NUM_CABLES >= 4
        extern struct usb_string_descriptor_struct usb_string_midi_port4
                __attribute__ ((weak, alias("usb_string_midi_port4_default")));
#endif
#if MIDI_NUM_CABLES >= 5
        extern struct usb_string_descriptor_struct usb_string_midi_port5
                __attribute__ ((weak, alias("usb_string_midi_port5_default")));
#endif
#if MIDI_NUM_CABLES >= 6
        extern struct usb_string_descriptor_struct usb_string_midi_port6
                __attribute__ ((weak, alias("usb_string_midi_port6_default")));
#endif
#if MIDI_NUM_CABLES >= 7
        extern struct usb_string_descriptor_struct usb_string_midi_port7
                __attribute__ ((weak, alias("usb_string_midi_port7_default")));
#endif
#if MIDI_NUM_CABLES >= 8
        extern struct usb_string_descriptor_struct usb_string_midi_port8
                __attribute__ ((weak, alias("usb_string_midi_port8_default")));
#endif
#if MIDI_NUM_CABLES >= 9
        extern struct usb_string_descriptor_struct usb_string_midi_port9
                __attribute__ ((weak, alias("usb_string_midi_port9_default")));
#endif
#if MIDI_NUM_CABLES >= 10
        extern struct usb_string_descriptor_struct usb_string_midi_port10
                __attribute__ ((weak, alias("usb_string_midi_port10_default")));
#endif
#if MIDI_NUM_CABLES >= 11
        extern struct usb_string_descriptor_struct usb_string_midi_port11
                __attribute__ ((weak, alias("usb_string_midi_port11_default")));
#endif
#if MIDI_NUM_CABLES >= 12
        extern struct usb_string_descriptor_struct usb_string_midi_port12
                __attribute__ ((weak, alias("usb_string_midi_port12_default")));
#endif
#if MIDI_NUM_CABLES >= 13
        extern struct usb_string_descriptor_struct usb_string_midi_port13
                __attribute__ ((weak, alias("usb_string_midi_port13_default")));
#endif
#if MIDI_NUM_CABLES >= 14
        extern struct usb_string_descriptor_struct usb_string_midi_port14
                __attribute__ ((weak, alias("usb_string_midi_port14_default")));
#endif
#if MIDI_NUM_CABLES >= 15
        extern struct usb_string_descriptor_struct usb_string_midi_port15
                __attribute__ ((weak, alias("usb_string_midi_port15_default")));
#endif
#if MIDI_NUM_CABLES >= 16
        extern struct usb_string_descriptor_struct usb_string_midi_port16
                __attribute__ ((weak, alias("usb_string_midi_port16_default")));
#endif
#endif

struct usb_string_descriptor_struct string0 = {
        4,
        3,
        {0x0409}
};

struct usb_string_descriptor_struct usb_string_manufacturer_name_default = {
        2 + MANUFACTURER_NAME_LEN * 2,
        3,
        MANUFACTURER_NAME
};
struct usb_string_descriptor_struct usb_string_product_name_default = {
	2 + PRODUCT_NAME_LEN * 2,
        3,
        PRODUCT_NAME
};
struct usb_string_descriptor_struct usb_string_serial_number_default = {
        12,
        3,
        {0,0,0,0,0,0,0,0,0,0}
};
#ifdef MIDI_INTERFACE
        struct usb_string_descriptor_struct usb_string_midi_port1_default = {
                14,
                3,
                {'P','o','r','t',' ','1'}
        };
#if MIDI_NUM_CABLES >= 2
        struct usb_string_descriptor_struct usb_string_midi_port2_default = {
                14,
                3,
                {'P','o','r','t',' ','2'}
        };
#endif
#if MIDI_NUM_CABLES >= 3
        struct usb_string_descriptor_struct usb_string_midi_port3_default = {
                14,
                3,
                {'P','o','r','t',' ','3'}
        };
#endif
#if MIDI_NUM_CABLES >= 4
        struct usb_string_descriptor_struct usb_string_midi_port4_default = {
                14,
                3,
                {'P','o','r','t',' ','4'}
        };
#endif
#if MIDI_NUM_CABLES >= 5
        struct usb_string_descriptor_struct usb_string_midi_port5_default = {
                14,
                3,
                {'P','o','r','t',' ','5'}
        };
#endif
#if MIDI_NUM_CABLES >= 6
        struct usb_string_descriptor_struct usb_string_midi_port6_default = {
                14,
                3,
                {'P','o','r','t',' ','6'}
        };
#endif
#if MIDI_NUM_CABLES >= 7
        struct usb_string_descriptor_struct usb_string_midi_port7_default = {
                14,
                3,
                {'P','o','r','t',' ','7'}
        };
#endif
#if MIDI_NUM_CABLES >= 8
        struct usb_string_descriptor_struct usb_string_midi_port8_default = {
                14,
                3,
                {'P','o','r','t',' ','8'}
        };
#endif
#if MIDI_NUM_CABLES >= 9
        struct usb_string_descriptor_struct usb_string_midi_port9_default = {
                14,
                3,
                {'P','o','r','t',' ','9'}
        };
#endif
#if MIDI_NUM_CABLES >= 10
        struct usb_string_descriptor_struct usb_string_midi_port10_default = {
                16,
                3,
                {'P','o','r','t',' ','1','0'}
        };
#endif
#if MIDI_NUM_CABLES >= 11
        struct usb_string_descriptor_struct usb_string_midi_port11_default = {
                16,
                3,
                {'P','o','r','t',' ','1','1'}
        };
#endif
#if MIDI_NUM_CABLES >= 12
        struct usb_string_descriptor_struct usb_string_midi_port12_default = {
                16,
                3,
                {'P','o','r','t',' ','1','2'}
        };
#endif
#if MIDI_NUM_CABLES >= 13
        struct usb_string_descriptor_struct usb_string_midi_port13_default = {
                16,
                3,
                {'P','o','r','t',' ','1','3'}
        };
#endif
#if MIDI_NUM_CABLES >= 14
        struct usb_string_descriptor_struct usb_string_midi_port14_default = {
                16,
                3,
                {'P','o','r','t',' ','1','4'}
        };
#endif
#if MIDI_NUM_CABLES >= 15
        struct usb_string_descriptor_struct usb_string_midi_port15_default = {
                16,
                3,
                {'P','o','r','t',' ','1','5'}
        };
#endif
#if MIDI_NUM_CABLES >= 16
        struct usb_string_descriptor_struct usb_string_midi_port16_default = {
                16,
                3,
                {'P','o','r','t',' ','1','6'}
        };
#endif
#endif

#ifdef MTP_INTERFACE
struct usb_string_descriptor_struct usb_string_mtp = {
	2 + 3 * 2,
	3,
	{'M','T','P'}
};
#endif

void usb_init_serialnumber(void)
{
	char buf[11];
	uint32_t i, num;

	__disable_irq();
#if defined(HAS_KINETIS_FLASH_FTFA) || defined(HAS_KINETIS_FLASH_FTFL)
	FTFL_FSTAT = FTFL_FSTAT_RDCOLERR | FTFL_FSTAT_ACCERR | FTFL_FSTAT_FPVIOL;
	FTFL_FCCOB0 = 0x41;
	FTFL_FCCOB1 = 15;
	FTFL_FSTAT = FTFL_FSTAT_CCIF;
	while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF)) ; // wait
	num = *(uint32_t *)&FTFL_FCCOB7;
#elif defined(HAS_KINETIS_FLASH_FTFE)
	kinetis_hsrun_disable();
	FTFL_FSTAT = FTFL_FSTAT_RDCOLERR | FTFL_FSTAT_ACCERR | FTFL_FSTAT_FPVIOL;
	*(uint32_t *)&FTFL_FCCOB3 = 0x41070000;
	FTFL_FSTAT = FTFL_FSTAT_CCIF;
	while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF)) ; // wait
	num = *(uint32_t *)&FTFL_FCCOBB;
	kinetis_hsrun_enable();
#endif
	__enable_irq();
	// add extra zero to work around OS-X CDC-ACM driver bug
	if (num < 10000000) num = num * 10;
	ultoa(num, buf, 10);
	for (i=0; i<10; i++) {
		char c = buf[i];
		if (!c) break;
		usb_string_serial_number_default.wString[i] = c;
	}
	usb_string_serial_number_default.bLength = i * 2 + 2;
}


// **************************************************************
//   Descriptors List
// **************************************************************

// This table provides access to all the descriptor data above.

const usb_descriptor_list_t usb_descriptor_list[] = {
	//wValue, wIndex, address,          length
	{0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
	{0x0200, 0x0000, config_descriptor, sizeof(config_descriptor)},
#ifdef SEREMU_INTERFACE
	{0x2200, SEREMU_INTERFACE, seremu_report_desc, sizeof(seremu_report_desc)},
	{0x2100, SEREMU_INTERFACE, config_descriptor+SEREMU_HID_DESC_OFFSET, 9},
#endif
#ifdef KEYBOARD_INTERFACE
        {0x2200, KEYBOARD_INTERFACE, keyboard_report_desc, sizeof(keyboard_report_desc)},
        {0x2100, KEYBOARD_INTERFACE, config_descriptor+KEYBOARD_HID_DESC_OFFSET, 9},
#endif
#ifdef MOUSE_INTERFACE
        {0x2200, MOUSE_INTERFACE, mouse_report_desc, sizeof(mouse_report_desc)},
        {0x2100, MOUSE_INTERFACE, config_descriptor+MOUSE_HID_DESC_OFFSET, 9},
#endif
#ifdef JOYSTICK_INTERFACE
        {0x2200, JOYSTICK_INTERFACE, joystick_report_desc, sizeof(joystick_report_desc)},
        {0x2100, JOYSTICK_INTERFACE, config_descriptor+JOYSTICK_HID_DESC_OFFSET, 9},
#endif
#ifdef RAWHID_INTERFACE
	{0x2200, RAWHID_INTERFACE, rawhid_report_desc, sizeof(rawhid_report_desc)},
	{0x2100, RAWHID_INTERFACE, config_descriptor+RAWHID_HID_DESC_OFFSET, 9},
#endif
#ifdef FLIGHTSIM_INTERFACE
	{0x2200, FLIGHTSIM_INTERFACE, flightsim_report_desc, sizeof(flightsim_report_desc)},
	{0x2100, FLIGHTSIM_INTERFACE, config_descriptor+FLIGHTSIM_HID_DESC_OFFSET, 9},
#endif
#ifdef KEYMEDIA_INTERFACE
        {0x2200, KEYMEDIA_INTERFACE, keymedia_report_desc, sizeof(keymedia_report_desc)},
        {0x2100, KEYMEDIA_INTERFACE, config_descriptor+KEYMEDIA_HID_DESC_OFFSET, 9},
#endif
#ifdef MULTITOUCH_INTERFACE
        {0x2200, MULTITOUCH_INTERFACE, multitouch_report_desc, sizeof(multitouch_report_desc)},
        {0x2100, MULTITOUCH_INTERFACE, config_descriptor+MULTITOUCH_HID_DESC_OFFSET, 9},
#endif
#ifdef MTP_INTERFACE
	{0x0304, 0x0409, (const uint8_t *)&usb_string_mtp, 0},
#endif
#ifdef MIDI_INTERFACE
	{0x0305, 0x0409, (const uint8_t *)&usb_string_midi_port1, 0},
#if MIDI_NUM_CABLES >= 2
	{0x0306, 0x0409, (const uint8_t *)&usb_string_midi_port2, 0},
#endif
#if MIDI_NUM_CABLES >= 3
	{0x0307, 0x0409, (const uint8_t *)&usb_string_midi_port3, 0},
#endif
#if MIDI_NUM_CABLES >= 4
	{0x0308, 0x0409, (const uint8_t *)&usb_string_midi_port4, 0},
#endif
#if MIDI_NUM_CABLES >= 5
	{0x0309, 0x0409, (const uint8_t *)&usb_string_midi_port5, 0},
#endif
#if MIDI_NUM_CABLES >= 6
	{0x030A, 0x0409, (const uint8_t *)&usb_string_midi_port6, 0},
#endif
#if MIDI_NUM_CABLES >= 7
	{0x030B, 0x0409, (const uint8_t *)&usb_string_midi_port7, 0},
#endif
#if MIDI_NUM_CABLES >= 8
	{0x030C, 0x0409, (const uint8_t *)&usb_string_midi_port8, 0},
#endif
#if MIDI_NUM_CABLES >= 9
	{0x030D, 0x0409, (const uint8_t *)&usb_string_midi_port9, 0},
#endif
#if MIDI_NUM_CABLES >= 10
	{0x030E, 0x0409, (const uint8_t *)&usb_string_midi_port10, 0},
#endif
#if MIDI_NUM_CABLES >= 11
	{0x030F, 0x0409, (const uint8_t *)&usb_string_midi_port11, 0},
#endif
#if MIDI_NUM_CABLES >= 12
	{0x0310, 0x0409, (const uint8_t *)&usb_string_midi_port12, 0},
#endif
#if MIDI_NUM_CABLES >= 13
	{0x0311, 0x0409, (const uint8_t *)&usb_string_midi_port13, 0},
#endif
#if MIDI_NUM_CABLES >= 14
	{0x0312, 0x0409, (const uint8_t *)&usb_string_midi_port14, 0},
#endif
#if MIDI_NUM_CABLES >= 15
	{0x0313, 0x0409, (const uint8_t *)&usb_string_midi_port15, 0},
#endif
#if MIDI_NUM_CABLES >= 16
	{0x0314, 0x0409, (const uint8_t *)&usb_string_midi_port16, 0},
#endif
#endif
        {0x0300, 0x0000, (const uint8_t *)&string0, 0},
        {0x0301, 0x0409, (const uint8_t *)&usb_string_manufacturer_name, 0},
        {0x0302, 0x0409, (const uint8_t *)&usb_string_product_name, 0},
        {0x0303, 0x0409, (const uint8_t *)&usb_string_serial_number, 0},
	{0, 0, NULL, 0}
};


// **************************************************************
//   Endpoint Configuration
// **************************************************************

#if 0
// 0x00 = not used
// 0x19 = Recieve only
// 0x15 = Transmit only
// 0x1D = Transmit & Recieve
//
const uint8_t usb_endpoint_config_table[NUM_ENDPOINTS] =
{
	0x00, 0x15, 0x19, 0x15, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#endif


const uint8_t usb_endpoint_config_table[NUM_ENDPOINTS] =
{
#if (defined(ENDPOINT1_CONFIG) && NUM_ENDPOINTS >= 1)
	ENDPOINT1_CONFIG,
#elif (NUM_ENDPOINTS >= 1)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT2_CONFIG) && NUM_ENDPOINTS >= 2)
	ENDPOINT2_CONFIG,
#elif (NUM_ENDPOINTS >= 2)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT3_CONFIG) && NUM_ENDPOINTS >= 3)
	ENDPOINT3_CONFIG,
#elif (NUM_ENDPOINTS >= 3)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT4_CONFIG) && NUM_ENDPOINTS >= 4)
	ENDPOINT4_CONFIG,
#elif (NUM_ENDPOINTS >= 4)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT5_CONFIG) && NUM_ENDPOINTS >= 5)
	ENDPOINT5_CONFIG,
#elif (NUM_ENDPOINTS >= 5)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT6_CONFIG) && NUM_ENDPOINTS >= 6)
	ENDPOINT6_CONFIG,
#elif (NUM_ENDPOINTS >= 6)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT7_CONFIG) && NUM_ENDPOINTS >= 7)
	ENDPOINT7_CONFIG,
#elif (NUM_ENDPOINTS >= 7)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT8_CONFIG) && NUM_ENDPOINTS >= 8)
	ENDPOINT8_CONFIG,
#elif (NUM_ENDPOINTS >= 8)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT9_CONFIG) && NUM_ENDPOINTS >= 9)
	ENDPOINT9_CONFIG,
#elif (NUM_ENDPOINTS >= 9)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT10_CONFIG) && NUM_ENDPOINTS >= 10)
	ENDPOINT10_CONFIG,
#elif (NUM_ENDPOINTS >= 10)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT11_CONFIG) && NUM_ENDPOINTS >= 11)
	ENDPOINT11_CONFIG,
#elif (NUM_ENDPOINTS >= 11)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT12_CONFIG) && NUM_ENDPOINTS >= 12)
	ENDPOINT12_CONFIG,
#elif (NUM_ENDPOINTS >= 12)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT13_CONFIG) && NUM_ENDPOINTS >= 13)
	ENDPOINT13_CONFIG,
#elif (NUM_ENDPOINTS >= 13)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT14_CONFIG) && NUM_ENDPOINTS >= 14)
	ENDPOINT14_CONFIG,
#elif (NUM_ENDPOINTS >= 14)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT15_CONFIG) && NUM_ENDPOINTS >= 15)
	ENDPOINT15_CONFIG,
#elif (NUM_ENDPOINTS >= 15)
	ENDPOINT_UNUSED,
#endif
};


#endif // NUM_ENDPOINTS
#endif // F_CPU >= 20 MHz
