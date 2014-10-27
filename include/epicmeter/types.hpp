#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <memory>

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
public:
    enum { size = 64 };
    typedef std::array<double, size> samples_t;

private:
    class impl;
    std::unique_ptr<impl> d;

public:
    stats_t() = delete;
    explicit stats_t(const samples_t& samples);
    stats_t(const stats_t& other);
    stats_t(stats_t&& other);
    ~stats_t();

    stats_t& operator=(stats_t&& other);

    double median() const;
    double median(const samples_t& samples) const;
    double median_abs_dev() const;
    double median_abs_dev_pct() const;

    double min() const;
    double max() const;
};

} // namespace epicmeter
