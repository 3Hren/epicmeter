#include <ticktack/benchmark.hpp>

int main(int, char**) {
    auto& benchmark = ticktack::benchmarker_t::instance();
    benchmark.run();
    return 0;
}
