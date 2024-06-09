#include <iostream>
#include <libusb-1.0/libusb.h>

void check_libusb_error(int r, const char* msg) {
    if (r < 0) {
        std::cerr << msg << ": " << libusb_error_name(r) << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    libusb_device_handle *handle;
    libusb_context *ctx = nullptr;
    int r;

    // Initialize libusb
    r = libusb_init(&ctx);
    check_libusb_error(r, "Failed to initialize libusb");

    // Open the device
    handle = libusb_open_device_with_vid_pid(ctx, 0x05EF, 0x020A); // Replace with your device's VID and PID
    if (!handle) {
        std::cerr << "Unable to open USB device" << std::endl;
        libusb_exit(ctx);
        exit(EXIT_FAILURE);
    }

    // Set the configuration (usually 1 for single configuration devices)
    r = libusb_set_configuration(handle, 1);
    if (r == LIBUSB_ERROR_BUSY) {
        std::cerr << "Configuration already set or device is in use" << std::endl;
    } else {
        check_libusb_error(r, "Failed to set configuration");
    }

    // Claim the interface (usually 0 for single interface devices)
    r = libusb_claim_interface(handle, 0);
    check_libusb_error(r, "Failed to claim interface");

    std::cout << "Device opened and interface claimed successfully." << std::endl;

    // Reading data from IN endpoint
    unsigned char data[16];
    int transferred;
    r = libusb_interrupt_transfer(handle, LIBUSB_ENDPOINT_IN | 2, data, sizeof(data), &transferred, 1000); // IN endpoint
    if (r == LIBUSB_SUCCESS) {
        std::cout << "Data received: ";
        for (int i = 0; i < transferred; ++i) {
            std::cout << std::hex << static_cast<int>(data[i]) << " ";
        }
        std::cout << std::dec << std::endl;
    } else {
        std::cerr << "Failed to read data: " << libusb_error_name(r) << std::endl;
    }

    // Writing data to OUT endpoint
    unsigned char outData[32] = { /* data to send */ };
    r = libusb_interrupt_transfer(handle, LIBUSB_ENDPOINT_OUT | 1, outData, sizeof(outData), &transferred, 1000); // OUT endpoint
    if (r == LIBUSB_SUCCESS) {
        std::cout << "Data sent: " << transferred << " bytes." << std::endl;
    } else {
        std::cerr << "Failed to send data: " << libusb_error_name(r) << std::endl;
    }

    // Clear endpoint stalls if any
    r = libusb_clear_halt(handle, LIBUSB_ENDPOINT_IN | 2);
    if (r != LIBUSB_SUCCESS) {
        std::cerr << "Failed to clear halt: " << libusb_error_name(r) << std::endl;
    }

    // Release the interface and close the device
    libusb_release_interface(handle, 0);
    libusb_close(handle);
    libusb_exit(ctx);

    return 0;
}
