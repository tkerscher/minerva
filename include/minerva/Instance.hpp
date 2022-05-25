#pragma once

#include <memory>

#include "minerva/config.hpp"

namespace minerva {
    //forward internal
    namespace vulkan {
        struct Instance;
    }
    //handle
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
}
