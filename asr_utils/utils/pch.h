#ifndef UTILS_PCH_H
#define UTILS_PCH_H

#include <asr.h>

#ifdef __cplusplus

#include <cstdint>
#include <cstdio>
#include <functional>
#include <format>
#include <string>
#include <string_view>

using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::int8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

#if __cplusplus > 202002L
using std::float32_t;
using std::float64_t;
#else

// NOLINTBEGIN(readability-magic-numbers)
static_assert(std::numeric_limits<float>::is_iec559 && std::numeric_limits<float>::digits == 24,
              "float is not ieee 32-bit");
static_assert(std::numeric_limits<double>::is_iec559 && std::numeric_limits<double>::digits == 53,
              "double is not ieee 64-bit");
// NOLINTEND(readability-magic-numbers)

using float32_t = float;
using float64_t = double;

#endif

#endif

#endif /* UTILS_PCH_H */
