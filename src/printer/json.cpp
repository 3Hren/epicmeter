#include <cmath>

#include <boost/lexical_cast.hpp>

#include "ticktack/benchmark.hpp"
#include "ticktack/printer/json.hpp"

using namespace ticktack;

void json_printer_t::start() {
    printf("{");
    fflush(stdout);
}

void json_printer_t::suite_start(const std::string& name, const suite_t& suite) {
    printf("\"%s\":{", name.c_str());
    fflush(stdout);
}

void json_printer_t::relative_pass(const std::string& name,
                                    const analyzed_t& info_b,
                                    const analyzed_t& info)
{
    printf("\"%s\":{\"elapsed\":%f,\"iters/sec\":%f},", name.c_str(), 100 * (2 - info.min / info_b.min), 1e9 / info.min);
    fflush(stdout);
}

void json_printer_t::pass(const std::string& name, const analyzed_t& info) {
    printf("\"%s\":{\"elapsed\":%f,\"iters/sec\":%f},", name.c_str(), info.min, 1e9 / info.min);
    fflush(stdout);
}

void json_printer_t::suite_complete(const std::string&, long long elapsed) {
    printf("\"suite total\":%s},", boost::lexical_cast<std::string>(elapsed).c_str());
    fflush(stdout);
}

void json_printer_t::complete(long long elapsed) {
    printf("\"global total\":%s}", boost::lexical_cast<std::string>(elapsed).c_str());
    fflush(stdout);
}
