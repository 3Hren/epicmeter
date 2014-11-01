#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

namespace epicmeter {

typedef std::chrono::high_resolution_clock clock_type;

struct nanosecond_type {
    typedef std::uint64_t value_type;

    value_type v;

    nanosecond_type() : v(0) {}
    nanosecond_type(value_type v) : v(v) {}
};

struct iteration_type {
    typedef std::uint64_t value_type;

    value_type v;

    iteration_type() : v(0) {}
    iteration_type(value_type v) : v(v) {}

    inline iteration_type& operator+=(const iteration_type& other) {
        v += other.v;
        return *this;
    }
};

class stats_t {
private:
    class impl;
    std::unique_ptr<impl> d;

public:
    stats_t(const std::vector<double>& samples);

    stats_t(const stats_t& other);
    stats_t(stats_t&& other);
    ~stats_t();

    stats_t& operator=(stats_t&& other);

    double min() const;
    double max() const;
    double median() const;
};

} // namespace epicmeter
