#include "epicmeter/output/table.hpp"

#include <cmath>

#include <iomanip>

#include "epicmeter/compiler.hpp"

using namespace epicmeter;
using namespace epicmeter::output;

namespace epicmeter {

namespace detail {

namespace candy {

struct scale_table_t {
    double scale;
    const char* suffix;
};

static const scale_table_t METRIC_SUFFIXES[] {
    { 1E24,  "Y" },
    { 1E21,  "Z" },
    { 1E18,  "E" },
    { 1E15,  "P" },
    { 1E12,  "T" },
    { 1E9,   "G" },
    { 1E6,   "M" },
    { 1E3,   "K" },
    { 1,     "" },
    { 1E-3,  "m" },
    { 1E-6,  "u" },
    { 1E-9,  "n" },
    { 1E-12, "p" },
    { 1E-15, "f" },
    { 1E-18, "a" },
    { 1E-21, "z" },
    { 1E-24, "y" },
    { 0, nullptr }
};

static const scale_table_t TIME_SUFFIXES[] {
    { 365.25 * 24 * 3600, "years" },
    { 24 * 3600,          "days"  },
    { 3600,               "hr"    },
    { 60,                 "min"   },
    { 1,                  "s"     },
    { 1E-3,               "ms"    },
    { 1E-6,               "us"    },
    { 1E-9,               "ns"    },
    { 1E-12,              "ps"    },
    { 1E-15,              "fs"    },
    { 0, nullptr                  }
};

static
inline
std::string
readable(double n, const scale_table_t* table, std::function<std::string(const scale_table_t*)> format) {
    if (std::isinf(n)) {
        if (n > 0) {
            return "+inf";
        } else {
            return "-inf";
        }
    }

    if (std::isnan(n)) {
        return "NaN";
    }

    const double v = std::abs(n);
    const scale_table_t* scale = table;
    while (v < scale[0].scale && scale[1].suffix != nullptr) {
        ++scale;
    }

    return format(scale);
}

static
inline
std::string
normal(double n, unsigned int decimals, const scale_table_t* scale) {
    char buffer[64];
    snprintf(buffer, 64, "%.*f%s", decimals, n / scale->scale, scale->suffix);
    return buffer;
}

static
inline
std::string
ranged(double median, double range, unsigned int decimals, const scale_table_t* scale) {
    char buffer[64];
    snprintf(buffer, 64, "%7.*f ± %3.1f%s", decimals, median / scale->scale, range / scale->scale, scale->suffix);
    return buffer;
}

static
inline
std::string
time(double n, unsigned int decimals) {
    return readable(n, TIME_SUFFIXES, std::bind(&normal, n, decimals, std::placeholders::_1));
}

static
inline
std::string
time(double median, double range) {
    return readable(median, TIME_SUFFIXES, std::bind(&ranged, median, range, 3, std::placeholders::_1));
}

} // namespace candy

} // namespace detail

} // namespace epicmeter

table_t::table_t(std::ostream& stream) :
    stream(stream)
{}

void table_t::global(std::size_t count) {
    stream << "[==========] GLOBAL BENCHMARK ENVIRONMENT SET-UP."
           << std::setw(27) << "[RELATIVE] "
           << "[    TIME/ITER    ] "
           << "[ ITERS/SEC ]"
           << std::endl;
    stream << "[----------] PREPARING " << count << " NAMESPACES ..."
           << std::endl;
}

void table_t::global(nanosecond_type) {
    stream << "[==========]" << std::endl;
}

void table_t::package(const std::string& ns, std::size_t count) {
    stream << "[----------] "
           << count << " benchmark" << (count > 1 ? "s " : " ")
           << "from " << ns
           << std::endl;
}

void table_t::package(nanosecond_type) {
    stream << "[----------]" << std::endl;
}

void table_t::benchmark(const std::string& cs) {
    current = cs;
}

void table_t::benchmark(const stats_t& stats) {
    stream << "[ ABSOLUTE ] "
           << std::setw(52) << std::left << current
           << std::setw(11) << ""
           << std::setw(19) << std::right << detail::candy::time(stats.median() / 1e9, (stats.max() - stats.min()) / 1e9)
           << std::setw(14) << std::fixed << std::setprecision(1) << (1e9 / stats.min())
           << std::endl;
}

void table_t::benchmark(const stats_t& stats, const stats_t& baseline) {
    stream << "[ RELATIVE ] "
           << std::setw(52) << std::left << current
           << std::setw(8) << std::right << std::fixed << std::setprecision(1)
           << 100 * baseline.min() / stats.min() << "%  "
           << std::setw(19) << std::right
           << detail::candy::time(stats.median() / 1e9, (stats.max() - stats.min()) / 1e9)
           << std::setw(14) << std::fixed << std::setprecision(1)
           << (1e9 / stats.min())
           << std::endl;
}
