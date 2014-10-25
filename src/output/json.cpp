#include "epicmeter/output/json.hpp"

#include "epicmeter/benchmark.hpp"

using namespace epicmeter;
using namespace epicmeter::output;

json_t::json_t(std::ostream &stream) :
    stream(stream)
{}

void json_t::global(std::size_t count) {
    stream << "{";
}

void json_t::global(nanosecond_type elapsed) {
    stream << "\"total\":" << elapsed.v << "}";
}

void json_t::package(const std::string& ns, std::size_t) {
    stream << "\"" << ns << "\":{";
}

void json_t::package(nanosecond_type elapsed) {
    stream << "\"total\":" << elapsed.v << "},";
}

void json_t::benchmark(const std::string& cs) {
    stream << "\"" << cs << "\":{";
}

void json_t::benchmark(const stats_t& stats) {
    stream << "\"elapsed\":" << stats.min() << ",\"rps\":" << 1e9 / stats.min() << "},";
}

void json_t::benchmark(const stats_t& stats, const stats_t&) {
    stream << "\"elapsed\":" << stats.min() << ",\"rps\":" << 1e9 / stats.min() << "},";
}
