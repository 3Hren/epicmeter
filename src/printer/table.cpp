#include <cmath>

#include "ticktack/benchmark.hpp"
#include "ticktack/printer/table.hpp"

using namespace ticktack;

namespace ticktack {

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
readable(double n, unsigned int decimals, const scale_table_t* table) {
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

    const double scaled = n / scale->scale;
    char buffer[64];
    snprintf(buffer, 64, "%.*f%s", decimals, scaled, scale->suffix);
    return buffer;
}

static
inline
std::string
time(double n, unsigned int decimals) {
    return readable(n, decimals, TIME_SUFFIXES);
}

static
inline
double
relative_percent(double baseline, double n) {
    return 100 * baseline / n;
}

} // namespace candy

} // namespace detail

} // namespace ticktack

void table_printer_t::start() {
    printf(
       "[==========] GLOBAL BENCHMARK ENVIRONMENT SET-UP.%27s %12s %12s\n",
       "[RELATIVE]",
       "[TIME/ITER]",
       "[ITERS/SEC]"
    );
    fflush(stdout);
}

void table_printer_t::suite_start(const std::string& name, const suite_t& suite) {
    printf(
        "[----------] %d benchmark from %s\n",
        static_cast<int>(suite.benchmarks.size()),
        name.c_str()
    );
    fflush(stdout);
}

void table_printer_t::relative_pass(const std::string& name,
                                    const analyzed_t& info_b,
                                    const analyzed_t& info)
{
    printf("[ RELATIVE ] %-50s %10.2f%% %12s %12.1f\n",
        name.c_str(),
        detail::candy::relative_percent(info_b.min, info.min),
        detail::candy::time(info.min / 1e9, 3).c_str(),
        1e9 / info.min
    );
    fflush(stdout);
}

void table_printer_t::pass(const std::string& name, const analyzed_t& info) {
    printf("[ ABSOLUTE ] %-50s             %12s %12.1f\n",
        name.c_str(),
        detail::candy::time(info.min / 1e9, 3).c_str(),
        1e9 / info.min
    );
    fflush(stdout);
}

void table_printer_t::suite_complete(const std::string&, long long elapsed) {
    printf("[----------]\n");
    fflush(stdout);
}

void table_printer_t::complete(long long elapsed) {
    printf("[==========]\n");
    fflush(stdout);
}
