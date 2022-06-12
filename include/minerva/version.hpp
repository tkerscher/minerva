#pragma once

#include <cstdint>

#include "minerva/config.hpp"

namespace minerva {
    /**
     * @brief The major version of this build
     * @ingroup core
     */
    MINERVA_API extern const uint32_t VERSION_MAJOR;
    /**
     * @brief The minor version of this build.
     * @ingroup core
     */
    MINERVA_API extern const uint32_t VERSION_MINOR;
    /**
     * @brief The path of this build
     * @ingroup core
     */
    MINERVA_API extern const uint32_t VERSION_PATCH;
}
