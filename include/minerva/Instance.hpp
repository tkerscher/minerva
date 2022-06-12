#pragma once

#include "minerva/Handles.hpp"
#include "minerva/config.hpp"

namespace minerva {
    /**
     * @brief Version of the app
     * @ingroup core
     * 
     * Struct describing the major, minor and patch of the application.
     * Might be used by the driver for optimization. 
     */
    struct AppVersion {
        union {
            struct {
                uint32_t major: 10; //!< Major version
                uint32_t minor: 10; //!< Minor version
                uint32_t patch: 12; //!< Patch version
            };
            uint32_t packed; //!< Major, minor, patch packed into 4 bytes
        };
    };
    static_assert(sizeof(AppVersion) == 4, "minerva::AppVersion must be packed into 4 bytes!");

    /**
     * @brief Create a Instance
     * @ingroup core
     * 
     * @return InstanceHandle handle to the created instance.
     */
    [[nodiscard]] MINERVA_API InstanceHandle createInstance();
    /**
     * @brief Create a Instance
     * @ingroup core
     * 
     * @param name App name
     * @param version app version
     * @return InstanceHandle handle to the created instance.
     */
    [[nodiscard]] MINERVA_API InstanceHandle createInstance(const char* name, AppVersion version);

    /**
     * @brief Create a Context
     * @ingroup core
     * 
     * Creates a context on the first found discrete GPU.
     * Falls back on the first found one if no discrete one was found.
     * 
     * @param instance Instance from which to create the context
     * @return ContextHandle handle to the created context
     */
    [[nodiscard]] MINERVA_API ContextHandle createContext(const InstanceHandle& instance);
    /**
     * @brief Create a Context
     * @ingroup core
     * 
     * Creates a context from the device specified by its vendor and device id.
     * 
     * @param instance Instance from which to create the context
     * @param vendorId Vendor id
     * @param deviceId Device id
     * @return ContextHandle handle to the created context
     */
    [[nodiscard]] MINERVA_API ContextHandle createContext(const InstanceHandle& instance,
        uint32_t vendorId, uint32_t deviceId);
}
