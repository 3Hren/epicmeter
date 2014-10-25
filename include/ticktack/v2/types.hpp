#pragma once

#include <array>
#include <chrono>
#include <cstdint>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

namespace ticktack {

struct nanosecond_type {
    typedef std::uint64_t value_type;

    value_type v;

    nanosecond_type() : v(0) {}
    nanosecond_type(value_type v) : v(v) {}
};

typedef std::chrono::high_resolution_clock clock_type;

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

struct stats_t {
    enum { size = 64 };

    typedef std::array<double, size> samples_t;

    samples_t samples;

    explicit stats_t(const samples_t& samples) :
        samples(samples)
    {}

    double median_abs_dev() const {
        const double median = this->median();
        std::vector<double> abs_devs;
        for (auto it = samples.begin(); it != samples.end(); ++it) {
            abs_devs.push_back(std::abs(median - *it));
        }

        return 1.4826 * this->median(abs_devs);
    }

    double median_abs_dev_pct() const {
        return (median_abs_dev() / median()) * 100.0;
    }

    double median() const {
        return median(samples);
    }

    template<class Collection>
    double median(const Collection& samples) const {
        boost::accumulators::accumulator_set<
            double,
            boost::accumulators::stats<
                boost::accumulators::tag::median
            >
        > acc;

        std::for_each(samples.begin(), samples.end(), std::bind(std::ref(acc), std::placeholders::_1));

        return boost::accumulators::median(acc);
    }

    double max() const {
        boost::accumulators::accumulator_set<
            double,
            boost::accumulators::stats<
                boost::accumulators::tag::max
            >
        > acc;
        std::for_each(samples.begin(), samples.end(), std::bind(std::ref(acc), std::placeholders::_1));

        return boost::accumulators::max(acc);
    }

    double min() const {
        boost::accumulators::accumulator_set<
            double,
            boost::accumulators::stats<
                boost::accumulators::tag::min
            >
        > acc;
        std::for_each(samples.begin(), samples.end(), std::bind(std::ref(acc), std::placeholders::_1));

        return boost::accumulators::min(acc);
    }
};

} // namespace ticktack
