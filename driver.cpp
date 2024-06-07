#include <iostream>
#include "libusb.h"

#define VENDOR_ID 0x05EF  // Replace XXXX with the vendor ID of your joystick
#define PRODUCT_ID 0x020A  // Replace XXXX with the product ID of your joystick
#define JOYSTICK_ENDPOINT_IN 0x81  // Endpoint address for input data

int main() {
    libusb_device_handle *devh = nullptr;
    int r;

    // Initialize libusb
    r = libusb_init(nullptr);
    if (r < 0) {
        std::cerr << "libusb_init error " << r << std::endl;
        return 1;
    }

    // Open joystick device by vendor and product ID
    devh = libusb_open_device_with_vid_pid(nullptr, VENDOR_ID, PRODUCT_ID);
    if (!devh) {
        std::cerr << "Failed to open joystick device" << std::endl;
        libusb_exit(nullptr);
        return 1;
    }

    // Claim interface
    r = libusb_claim_interface(devh, 0);
    if (r < 0) {
        std::cerr << "Failed to claim interface" << std::endl;
        libusb_close(devh);
        libusb_exit(nullptr);
        return 1;
    }

    // Continuously poll for joystick events
    while (true) {
        unsigned char data[8]; // Assume joystick data packet size is 8 bytes
        int transferred;

        // Read data from the joystick device
        r = libusb_interrupt_transfer(devh, JOYSTICK_ENDPOINT_IN, data, sizeof(data), &transferred, 1000);
        if (r == 0 && transferred == sizeof(data)) {
            // Process joystick button presses here
            // Example: check if the first byte contains button press information
            if (data[0] & 0x01) {
                std::cout << "Button 1 pressed" << std::endl;
            }
            // Additional button checks can be added similarly
        } else if (r != LIBUSB_ERROR_TIMEOUT) {
            std::cerr << "Error reading joystick data: " << libusb_error_name(r) << std::endl;
        }
    }

    // Release interface and close device
    libusb_release_interface(devh, 0);
    libusb_close(devh);
    libusb_exit(nullptr);

    return 0;
}