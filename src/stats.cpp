#include "ticktack/types.hpp"

using namespace ticktack;

stats_t::stats_t(const samples_t& samples) :
    samples(samples)
{}

double stats_t::median() const {
    return median(samples);
}

double stats_t::median(const samples_t& samples) const {
    boost::accumulators::accumulator_set<
        double,
        boost::accumulators::stats<
            boost::accumulators::tag::median
        >
    > acc;

    std::for_each(samples.begin(), samples.end(), std::bind(std::ref(acc), std::placeholders::_1));

    return boost::accumulators::median(acc);
}

double stats_t::median_abs_dev() const {
    const double median = this->median();
    samples_t abs_devs;
    for (std::size_t i = 0; i < samples.size(); ++i) {
        abs_devs[i] = std::abs(median - samples[i]);
    }

    return 1.4826 * this->median(abs_devs);
}

double stats_t::median_abs_dev_pct() const {
    return (median_abs_dev() / median()) * 100.0;
}

double stats_t::min() const {
    boost::accumulators::accumulator_set<
        double,
        boost::accumulators::stats<
            boost::accumulators::tag::min
        >
    > acc;
    std::for_each(samples.begin(), samples.end(), std::bind(std::ref(acc), std::placeholders::_1));

    return boost::accumulators::min(acc);
}

double stats_t::max() const {
    boost::accumulators::accumulator_set<
        double,
        boost::accumulators::stats<
            boost::accumulators::tag::max
        >
    > acc;
    std::for_each(samples.begin(), samples.end(), std::bind(std::ref(acc), std::placeholders::_1));

    return boost::accumulators::max(acc);
}
