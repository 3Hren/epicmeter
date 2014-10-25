#include <time.h>

#include <ticktack/v2/benchmark.hpp>

using namespace ticktack;

namespace {

void sleep_() {
    usleep(1000);
}

void postincrement() {
    int i = 0;
    i++;
}

void preincrement() {
    int i = 0;
    ++i;
    for (int u = 0; u <100; ++u) {
        i++;
    }
}

static builder_t builder1("Increment", "Using: i++", postincrement);
static builder_t builder2("Increment", "Using: ++i", preincrement, true);
static builder_t builder3("Sleep", "Usleep", sleep_);

} // namespace
