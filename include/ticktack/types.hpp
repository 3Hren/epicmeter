#pragma once

#include <chrono>
#include <cstdint>

namespace ticktack {

typedef std::uint64_t nanosecond_type;
typedef std::chrono::high_resolution_clock clock_type;

struct iteration_type {
    typedef std::uint64_t value_type;

    value_type v;

    iteration_type() : v(0) {}
    explicit iteration_type(value_type v) : v(v) {}
};

} // namespace ticktack
