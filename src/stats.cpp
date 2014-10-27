#include "epicmeter/types.hpp"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

using namespace epicmeter;

class stats_t::impl {
public:
    samples_t samples;
    boost::accumulators::accumulator_set<
        double,
        boost::accumulators::stats<
            boost::accumulators::tag::min,
            boost::accumulators::tag::max,
            boost::accumulators::tag::median
        >
    > acc;

    impl(const samples_t& samples) :
        samples(samples)
    {
        std::for_each(this->samples.begin(), this->samples.end(), std::bind(std::ref(acc), std::placeholders::_1));
    }

    impl(const impl& other) :
        samples(other.samples),
        acc(other.acc)
    {}
};

stats_t::stats_t(const samples_t& samples) :
    d(new impl(samples))
{}

stats_t::stats_t(const stats_t &other) :
    d(new impl(*other.d))
{}

stats_t::stats_t(stats_t&& other) :
    d(std::move(other.d))
{}

stats_t::~stats_t() {}

stats_t& stats_t::operator=(stats_t&& other) {
    d = std::move(other.d);
    return *this;
}

double stats_t::median() const {
    return boost::accumulators::median(d->acc);
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
    for (std::size_t i = 0; i < d->samples.size(); ++i) {
        abs_devs[i] = std::abs(median - d->samples[i]);
    }

    return 1.4826 * this->median(abs_devs);
}

double stats_t::median_abs_dev_pct() const {
    return (median_abs_dev() / median()) * 100.0;
}

double stats_t::min() const {
    return boost::accumulators::min(d->acc);
}

double stats_t::max() const {
    return boost::accumulators::max(d->acc);
}
