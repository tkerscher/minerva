#include <iostream>

#include "minerva/Device.hpp"
#include "minerva/Instance.hpp"
#include "minerva/version.hpp"

using namespace minerva;

int main() {
    //minerva info
    std::cout << "Minerva v" << VERSION_MAJOR << '.' << VERSION_MINOR << '.' << VERSION_PATCH << '\n';

    auto instance = createInstance("deviceinfo", { 1,0,0 });
    auto devices = enumerateDevices(instance);

    std::cout << devices.size() << " adequate device(s) found\n";
    std::cout << std::boolalpha;

    //iterate over devices
    for (auto& device : devices) {
        std::cout << "===============================================\n"
            << "Name:        " << device.name << '\n'
            << "Discrete:    " << device.isDiscrete << '\n'
            << std::hex
            << "VendorId:    0x" << device.vendorID << '\n'
            << "DeviceId:    0x" << device.deviceID << '\n';
    }
}
