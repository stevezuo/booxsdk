// Author: John

#ifndef BASE_DEVICE_H__
#define BASE_DEVICE_H__


// Make sure they are the same as defined in the udev rules.
// See /etc/udev/rules.d/10-naboo.rules for details.
const char * const SDMMC_DEVICE        = "/dev/sdmmc";
const char * const USB_DEVICE          = "/dev/usbdisk";

// The battery.
const char * const BATTERY_DEVICE      = "/proc/apm";
const char * const TOUCH_SCREEN_DEVICE = "/dev/ttymxc1";

const char * const USB_ROOT            = "/media/usb";
const char * const SDMMC_ROOT          = "/media/sd";
const char * const LIBRARY_ROOT        = "/media/flash";

// Default download directory
const char * const DOWNLOAD_ROOT       = "/media/sd/download";

// Battery level
static const int BATTERY_STATUS_CHARGING    = 0x01;
static const int BATTERY_STATUS_NORMAL      = 0x02;
static const int BATTERY_STATUS_WARNING     = 0x04;    ///< Low battery.
static const int BATTERY_STATUS_ALARM       = 0x08;    ///< Really low.
static const int BATTERY_STATUS_DANGEROUS   = 0x10;    ///< Shutdown now.

// Shutdown reason.
static const int SHUTDOWN_REASON_NONE        = 0;
static const int SHUTDOWN_REASON_LOW_BATTERY = 1;
static const int SHUTDOWN_REASON_USER_REQUEST = 2;

#endif  // BASE_DEVICE_H__
