#include <algorithm>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/min.hpp>

#include "ticktack/benchmark.hpp"
#include "ticktack/printer/table.hpp"

namespace ticktack {

namespace detail {

static
inline
analyzed_t
analyze(const std::vector<result_t>& results) {
    std::vector<double> times;
    times.reserve(results.size());
    for (auto it = results.begin(); it != results.end(); ++it) {
        times.push_back(double(it->total) / it->iters.v);
    }

    boost::accumulators::accumulator_set<
        double,
        boost::accumulators::features<
            boost::accumulators::tag::min
        >
    > acc;
    std::for_each(
        times.begin(),
        times.end(),
        std::bind(std::ref(acc), std::placeholders::_1)
    );

    analyzed_t analyzed {
        boost::accumulators::extract::min(acc)
    };
    return analyzed;
}

} // namespace detail

benchmarker_t& benchmarker_t::instance() {
    static benchmarker_t self;
    return self;
}

void
benchmarker_t::add_baseline(std::string suite,
                            std::string name,
                            runner::measurer_t function)
{
    BOOST_ASSERT(suites[suite].batch || suites[suite].benchmarks.empty());
    suites[suite].batch = true;
    suites[suite].benchmarks.emplace_back(runnable_t { name, function });
}

void
benchmarker_t::add_relative(std::string suite,
                            std::string name,
                            runner::measurer_t function)
{
    BOOST_ASSERT(suites[suite].batch && !suites[suite].benchmarks.empty());
    suites[suite].benchmarks.emplace_back(runnable_t { name, function });
}

void
benchmarker_t::add(std::string suite,
                   std::string name,
                   runner::measurer_t function)
{
    BOOST_ASSERT(!suites[suite].batch);
    suites[suite].benchmarks.emplace_back(runnable_t { name, function });
}

void benchmarker_t::set_watcher(std::unique_ptr<watcher_t> watcher) {
    this->watcher = std::move(watcher);
}

void benchmarker_t::run() {
    using std::chrono::duration_cast;

    clock_type clock;
    auto start = clock.now();
    watcher->start();
    for (auto it = suites.begin(); it != suites.end(); ++it) {
        auto suite_start = clock.now();
        watcher->suite_start(it->first, it->second);
        run(it->second);
        auto suite_elapsed = clock.now();
        watcher->suite_complete(
            it->first,
            duration_cast<std::chrono::nanoseconds>(suite_elapsed - suite_start).count()
        );
    }
    auto elapsed = clock.now();
    watcher->complete(
        duration_cast<std::chrono::nanoseconds>(elapsed - start).count()
    );
}

void benchmarker_t::run(suite_t& suite) {
    auto it = suite.benchmarks.begin();
    if (it == suite.benchmarks.end()) {
        return;
    }

    if (suite.batch) {
        auto info_b = detail::analyze(run(it->fn));
        watcher->relative_pass(it->name, info_b, info_b);

        while (++it != suite.benchmarks.end()) {
            auto info = detail::analyze(run(it->fn));
            watcher->relative_pass(it->name, info_b, info);
        }
    } else {
        do {
            auto info = detail::analyze(run(it->fn));
            watcher->pass(it->name, info);
        } while (++it != suite.benchmarks.end());
    }
}

std::vector<result_t>
benchmarker_t::run(const runner::measurer_t& fn) const {
    const std::uint32_t epochs = 128;

    clock_type clock;
    auto start = clock.now();

    std::vector<result_t> results;
    results.reserve(epochs);

    std::uint32_t epoch = 0;
    while (epoch < epochs) {
       double min = std::numeric_limits<double>::max();
       for (iteration_type iter = options.iters;
            iter.v < std::numeric_limits<iteration_type::value_type>::max();
            iter.v *= 2)
       {
           const result_t result = fn(iter);
           if (result.total < options.time.min) {
               continue;
           }

           if (epoch >= results.size()) {
               results.push_back(result);
           } else {
               const double elapsed = double(result.total) / result.iters.v;
               if (elapsed < min) {
                   min = elapsed;
                   results[epoch] = result;
               }
           }

           break;
       }

       auto elapsed = clock.now();
       if (std::chrono::duration_cast<
               std::chrono::nanoseconds
           >(elapsed - start).count() >= options.time.max)
       {
           break;
       }

       epoch++;
    }

    return results;
}

benchmarker_t::benchmarker_t()  :
    watcher(new table_printer_t())
{
    options.time.min = 1e8;
    options.time.max = 1e8;
    options.iters = iteration_type(1);
}

iteration_type runner::single_t::operator()() const {
    fn();
    return iteration_type(1);
}

iteration_type runner::pass_t::operator()(iteration_type times) const {
    fn(times);
    return times;
}

iteration_type runner::repeater_t::operator()(iteration_type times) const {
    iteration_type iters(0);
    while (times.v-- > 0) {
        iters.v += fn().v;
    }

    return iters;
}

result_t runner::measurer_t::operator()(iteration_type times) const {
    result_t result;

    clock_type clock;
    auto start = clock.now();
    result.iters = fn(times);
    auto elapsed = clock.now();

    result.total = std::chrono::duration_cast<
        std::chrono::nanoseconds
    >(elapsed - start).count();

    return result;
}

} // namespace ticktack
