#ifndef USB_CUSTOM_HID_H_
#define USB_CUSTOM_HID_H_

#include "custom_usb_names.h"
#include <custom_usb_desc.h>     

#include <inttypes.h>

// C language implementation
#ifdef __cplusplus
extern "C"
{
#endif
    int usb_customhid_recv(void *buffer, uint32_t timeout);
    int usb_customhid_available(void);
    int usb_customhid_send(const void *buffer, uint32_t timeout);
#ifdef __cplusplus
}
#endif

// C++ interface
#ifdef __cplusplus
class usb_custom_class
{
public:
    int available(void) { return usb_customhid_available(); }
    int recv(void *buffer, uint16_t timeout) { return usb_customhid_recv(buffer, timeout); }
    int send(const void *buffer, uint16_t timeout) { return usb_customhid_send(buffer, timeout); }
};

extern usb_custom_class CustomHID; // Declaration

#endif // __cplusplus
#endif // USB_CUSTOM_HID
