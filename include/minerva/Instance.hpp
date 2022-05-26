#pragma once

#include <memory>

#include "minerva/config.hpp"

namespace minerva {
    //forward internal
    namespace vulkan {
        struct Context;
        struct Instance;
    }
    //handle
    using ContextHandle  = std::unique_ptr<vulkan::Context, void(*)(vulkan::Context*)>;
    using InstanceHandle = std::unique_ptr<vulkan::Instance, void(*)(vulkan::Instance*)>;

    struct AppVersion {
        union {
            struct {
                uint32_t major: 10;
                uint32_t minor: 10;
                uint32_t patch: 12;
            };
            uint32_t packed;
        };
    };
    static_assert(sizeof(AppVersion) == 4, "minerva::AppVersion must be packed into 4 bytes!");

    [[nodiscard]] MINERVA_API InstanceHandle createInstance();
    [[nodiscard]] MINERVA_API InstanceHandle createInstance(const char* name, AppVersion version);

    [[nodiscard]] MINERVA_API ContextHandle createContext(const InstanceHandle& instance);
    [[nodiscard]] MINERVA_API ContextHandle createContext(const InstanceHandle& instance,
        uint32_t vendorId, uint32_t deviceId);
    [[nodiscard]] MINERVA_API ContextHandle createContext(const InstanceHandle& instance,
        const char* name, AppVersion version);
    [[nodiscard]] MINERVA_API ContextHandle createContext(const InstanceHandle& instance,
        const char* name, AppVersion version, uint32_t vendorId, uint32_t deviceId);
}
