#pragma once

#include <ostream>

#include "epicmeter/output/mod.hpp"

namespace epicmeter {

namespace output {

class table_t : public printer_t {
    std::ostream& stream;

    std::string current;

public:
    table_t(std::ostream& stream);

    void global(std::size_t count);
    void global(nanosecond_type elapsed);
    void package(const std::string& ns, std::size_t count);
    void package(nanosecond_type elapsed);
    void benchmark(const std::string& cs);
    void benchmark(const stats_t& stats);
    void benchmark(const stats_t& stats, const stats_t& baseline);
};

} // namespace output

} // namespace epicmeter
