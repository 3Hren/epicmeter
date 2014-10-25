#pragma once

#include <array>
#include <chrono>
#include <cstdint>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

namespace ticktack {

typedef std::uint64_t nanosecond_type;
typedef std::chrono::high_resolution_clock clock_type;

struct iteration_type {
    typedef std::uint64_t value_type;

    value_type v;

    iteration_type() : v(0) {}
    explicit iteration_type(value_type v) : v(v) {}

    inline iteration_type& operator+=(const iteration_type& other) {
        v += other.v;
        return *this;
    }
};

struct stats_t {
    std::array<double, 64> samples;

    stats_t() {}
    stats_t(const std::array<double, 64>& samples) :
        samples(samples)
    {}

    double median_abs_dev() const {
        auto med = median();
        std::vector<double> abs_devs;
        for (auto s : samples) {
            abs_devs.push_back(std::abs(med - s));
        }

        return 1.4826 * medi(abs_devs);
    }

    double median_abs_dev_pct() const {
        return (median_abs_dev() / median()) * 100.0;
    }

    double median() const {
        using namespace boost::accumulators;
        accumulator_set<double, stats<tag::median>> acc;
        for (auto s : samples) {
            acc(s);
        }
        return boost::accumulators::median(acc);
    }

    template<class T>
    double medi(const T& samples) const {
        using namespace boost::accumulators;
        accumulator_set<double, stats<tag::median>> acc;
        for (auto s : samples) {
            acc(s);
        }
        return boost::accumulators::median(acc);
    }

    double max() const {
        using namespace boost::accumulators;
        accumulator_set<double, stats<tag::max>> acc;
        for (auto s : samples) {
            acc(s);
        }
        return boost::accumulators::max(acc);
    }

    double min() const {
        using namespace boost::accumulators;
        accumulator_set<double, stats<tag::min>> acc;
        for (auto s : samples) {
            acc(s);
        }
        return boost::accumulators::min(acc);
    }
};

} // namespace ticktack
