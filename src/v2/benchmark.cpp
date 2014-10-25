#include "ticktack/v2/benchmark.hpp"

#include "ticktack/v2/output/table.hpp"

#include <iostream>

#include <boost/optional.hpp>

using namespace ticktack;

overlord_t::overlord_t() :
    out(new output::table_t(std::cout))
{
    options_.time.min = 1e8;
    options_.time.max = 1e9;
    options_.iters = iteration_type(1);
}

void overlord_t::run() {
    out->global(namespaces.size());
    for (auto it = namespaces.begin(); it != namespaces.end(); ++it) {
        run(it->first, it->second);
    }

    out->global(nanosecond_type(1e9));
}

namespace compare_by {

struct baseline {
    bool operator()(const benchmark_t& lhs, const benchmark_t& rhs) const {
        return lhs.baseline > rhs.baseline;
    }
};

} // namespace comparator

void overlord_t::run(const std::string& name, namespace_t ns) {
    std::stable_sort(ns.benchmarks.begin(), ns.benchmarks.end(), compare_by::baseline());

    out->package(name, ns.benchmarks.size());
    auto it = ns.benchmarks.begin();

    if (it != ns.benchmarks.end()) {
        if (it->baseline) {
            boost::optional<stats_t> baseline;
            for (; it != ns.benchmarks.end(); ++it) {
                run(*it, &baseline);
            }
        } else {
            for (; it != ns.benchmarks.end(); ++it) {
                run(*it);
            }
        }
    }
    out->package(nanosecond_type(1e6));
}

void overlord_t::run(const benchmark_t& benchmark) {
    out->benchmark(benchmark.description);
    out->benchmark(run(benchmark.fn));
}

void overlord_t::run(const benchmark_t& benchmark, boost::optional<stats_t>* baseline) {
    out->benchmark(benchmark.description);
    stats_t stats(run(benchmark.fn));
    if (!baseline->is_initialized()) {
        *baseline = stats;
    }
    out->benchmark(stats, baseline->get());
}

inline
double npi(const std::function<iteration_type(iteration_type)>& fn, iteration_type times) {
    auto started = clock_type::now();
    auto iters = fn(times);
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(clock_type::now() - started).count();

    return (double)elapsed / iters.v;
}

stats_t overlord_t::run(const std::function<iteration_type(iteration_type)>& fn) {
    std::array<double, 64> samples;
    std::fill(samples.begin(), samples.end(), 0.0);

    auto start = clock_type::now();

    iteration_type n(1); //TODO: determine min n hint.

    stats_t prev;
    while (true) {
        for (auto it = samples.begin(); it != samples.end(); ++it) {
            auto v = npi(fn, n);
//            std::cout << v << std::endl;
            *it = v;
        }

        auto curr = stats_t(samples);
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(clock_type::now() - start).count();
        if (elapsed > options_.time.min &&
                prev.median_abs_dev_pct() < 1.0
                && prev.median() - curr.median() < curr.median_abs_dev()) {
            return curr;
        }

        if (elapsed >= options_.time.max) {
            return curr;
        }

        n.v *= 2;
    }

    return prev;
}

namespace {

inline
iteration_type
single(std::function<void()> fn) {
    fn();
    return iteration_type(1);
}

inline
iteration_type
pass(std::function<void(iteration_type)> fn, iteration_type times) {
    fn(times);
    return times;
}

inline
iteration_type
repeater(std::function<iteration_type()> fn, iteration_type times) {
    iteration_type iters(0);
    while (times.v-- > 0) {
        iters += fn();
    }

    return iters;
}

} // namespace

std::function<iteration_type(iteration_type)>
detail::wrap(std::function<iteration_type(iteration_type)> fn) {
    return std::move(fn);
}

std::function<iteration_type(iteration_type)>
detail::wrap(std::function<void(iteration_type)> fn) {
    return std::bind(&pass, std::move(fn), std::placeholders::_1);
}

std::function<iteration_type(iteration_type)>
detail::wrap(std::function<iteration_type()> fn) {
    return std::bind(&repeater, std::move(fn), std::placeholders::_1);
}

std::function<iteration_type(iteration_type)>
detail::wrap(std::function<void()> fn) {
    return std::bind(
        &repeater,
        static_cast<std::function<iteration_type()>>(std::bind(&single, std::move(fn))),
        std::placeholders::_1
    );
}
