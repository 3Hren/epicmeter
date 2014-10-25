#pragma once

#include "ticktack/v2/types.hpp"

namespace ticktack {

namespace output {

class printer_t {
public:
    virtual ~printer_t() {}

    virtual void global(std::size_t count) = 0;
    virtual void global(nanosecond_type elapsed) = 0;
    virtual void package(const std::string& ns, std::size_t count) = 0;
    virtual void package(nanosecond_type elapsed) = 0;
    virtual void benchmark(const std::string& cs) = 0;
    virtual void benchmark(const stats_t& stats) = 0;
    virtual void benchmark(const stats_t& stats, const stats_t& baseline) = 0;
};

}

}
