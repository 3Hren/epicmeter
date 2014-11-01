#include <time.h>

#include <epicmeter/benchmark.hpp>

#include <unistd.h>

using namespace epicmeter;

//BENCHMARK(DatetimeGeneration, strftime, native, samples = 30; count = 100000; description = "Very long description") {
//}

//!=============================================================================
//! Common usage.
BENCHMARK(DatetimeFormatting, strftime) {
    std::time_t time = std::time(0);
    std::tm tm;
    localtime_r(&time, &tm);
    char buffer[64];
    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", &tm);
    compiler::do_not_optimize(buffer);
}

//!=============================================================================
//! Baseline support.
BENCHMARK_BASELINE(VectorRelative, push_back 10k elements) {
    std::vector<int> v;
    for (int i = 0; i < 10000; ++i) {
        v.push_back(i);
    }
}

//! This benchmark will be compared with the upper one. Note, that they both have the same namespace.
BENCHMARK(VectorRelative, push_back 10k elements reserved) {
    std::vector<int> v;
    v.reserve(10000);
    for (int i = 0; i < 10000; ++i) {
        v.push_back(i);
    }
}

//!=============================================================================
//! Sometimes you want to perform iterations yourself. In that case the library
//! provides you iteration count parameter.
BENCHMARK(DatetimeFormatting, strftime: N provided, n) {
    std::time_t time = std::time(0);
    std::tm tm;
    localtime_r(&time, &tm);
    char buffer[64];
    for (iteration_type::value_type i = 0; i < n.v; ++i) {
        strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", &tm);
        compiler::do_not_optimize(buffer);
    }
}

//!=============================================================================
//! Sometimes you want to perform iterations yourself and you know iteration
//! count. In that case you should return iteration count from function.
BENCHMARK_RETURN(DatetimeFormatting, strftime: N own) {
    iteration_type iters(10000);

    std::time_t time = std::time(0);
    std::tm tm;
    localtime_r(&time, &tm);
    char buffer[64];
    for (iteration_type::value_type i = 0; i < iters.v; ++i) {
        strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", &tm);
        compiler::do_not_optimize(buffer);
    }
    return iters;
}

//!=============================================================================
//! Benchmark with bound parameters.
void format_by_pattern(const char* format) {
    std::time_t time = std::time(0);
    std::tm tm;
    localtime_r(&time, &tm);
    char buffer[64];
    strftime(buffer, 64, format, &tm);
    compiler::do_not_optimize(buffer);
}
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern, "%Y-%m-%d");
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern, "%H:%M:%S");
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern, "%Y-%m-%d %H:%M:%S");

//! Benchmark with multiple bound parameters.
void format_by_pattern_multiple(const char* format, double pi) {
    std::time_t time = std::time(0);
    std::tm tm;
    localtime_r(&time, &tm);
    char buffer[64];
    strftime(buffer, 64, format, &tm);
    compiler::do_not_optimize(buffer);
}
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern_multiple, "%Y-%m-%d", 3.1415);
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern_multiple, "%H:%M:%S", 2 * 3.1415);
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern_multiple, "%Y-%m-%d %H:%M:%S", 3 * 3.1415);

//!=============================================================================
//! Benchmark with bound parameters and iteration count provided.
void format_by_pattern_provided(iteration_type iters, const char* format) {
    std::time_t time = std::time(0);
    std::tm tm;
    localtime_r(&time, &tm);
    char buffer[64];
    for (iteration_type::value_type i = 0; i < iters.v; ++i) {
        strftime(buffer, 64, format, &tm);
        compiler::do_not_optimize(buffer);
    }
}
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern_provided, "%Y-%m-%d");
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern_provided, "%H:%M:%S");
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern_provided, "%Y-%m-%d %H:%M:%S");

//!=============================================================================
//! Benchmark with bound parameters and iteration count returned.
iteration_type format_by_pattern_returned(const char* format) {
    iteration_type iters(10000);
    std::time_t time = std::time(0);
    std::tm tm;
    localtime_r(&time, &tm);
    char buffer[64];
    for (iteration_type::value_type i = 0; i < iters.v; ++i) {
        strftime(buffer, 64, format, &tm);
        compiler::do_not_optimize(buffer);
    }
    return iters;
}
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern_returned, "%Y-%m-%d");
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern_returned, "%H:%M:%S");
BENCHMARK_BOUND(DatetimeFormatting, format_by_pattern_returned, "%Y-%m-%d %H:%M:%S");
