#include "epicmeter/types.hpp"

#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/median.hpp>

using namespace epicmeter;

class stats_t::impl {
public:
    boost::accumulators::accumulator_set<
        double,
        boost::accumulators::stats<
            boost::accumulators::tag::min,
            boost::accumulators::tag::max,
            boost::accumulators::tag::median
        >
    > acc;

    impl(const std::vector<double>& samples) {
        std::for_each(samples.begin(), samples.end(), std::bind(std::ref(acc), std::placeholders::_1));
    }

    impl(const impl& other) :
        acc(other.acc)
    {}
};

stats_t::stats_t(const std::vector<double>& samples) :
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

double stats_t::min() const {
    return boost::accumulators::min(d->acc);
}

double stats_t::max() const {
    return boost::accumulators::max(d->acc);
}
