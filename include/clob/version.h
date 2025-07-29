/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <cstdint>

namespace clob {
/** Version Info **/
constexpr uint32_t VERSION_MAJOR{0};
constexpr uint32_t VERSION_MINOR{2};
constexpr uint32_t VERSION_PATCH{0};
constexpr uint32_t VERSION{VERSION_MAJOR * 10000 + VERSION_MINOR * 100 +
                           VERSION_PATCH};
} // namespace clob