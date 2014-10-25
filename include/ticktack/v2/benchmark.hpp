#pragma once

#include <array>
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>

#include "ticktack/v2/types.hpp"
#include "ticktack/v2/output/mod.hpp"

namespace boost { template<typename> class optional; }

namespace ticktack {

struct options_t {
    struct {
        nanosecond_type min;
        nanosecond_type max;
    } time;

    iteration_type iters;
};

// Один тест кейс и его статистику выполнения.
class benchmark_t {
public:
    bool baseline;
    std::string description;
    std::function<iteration_type(iteration_type)> fn;

//    std::array<std::tuple<double>, 64> npi; // Выборка -> | time/iter (mean +- dev) | iter/sec (mean +- dev) |
};

// Коллекция одного пространства имен тестов.
class namespace_t {
public:
    std::vector<benchmark_t> benchmarks;
};

// Класс для аггрегации и запуска бенчмарков.
class overlord_t {
    std::unordered_map<std::string, namespace_t> namespaces;

    options_t options_;
    std::unique_ptr<output::printer_t> out;

public:
    static overlord_t& instance() {
        static overlord_t self;
        return self;
    }

    void output(std::unique_ptr<output::printer_t> output) {
        this->out = std::move(output);
    }

    void options(options_t options) {
        this->options_ = options;
    }

    void add(std::string ns, std::string cs, bool baseline, std::function<iteration_type(iteration_type)> fn) {
        namespaces[ns].benchmarks.push_back(benchmark_t { baseline, cs, fn });
    }

    void run();
    void run(const std::string& name, namespace_t ns);
    void run(const benchmark_t& benchmark);
    void run(const benchmark_t& benchmark, boost::optional<stats_t>* baseline);
    stats_t run(const std::function<iteration_type(iteration_type)>& fn);

private:
    overlord_t();
};

namespace detail {

std::function<iteration_type(iteration_type)>
wrap(std::function<iteration_type(iteration_type)> fn);

//inline
//runner::measurer_t
//wrap(std::function<iteration_type()> fn) {
//    return runner::measurer_t { runner::repeater_t { std::move(fn) } };
//}

//inline
//runner::measurer_t
//wrap(std::function<void(iteration_type)> fn) {
//    return runner::measurer_t { runner::pass_t { std::move(fn) } };
//}

std::function<iteration_type(iteration_type)>
wrap(std::function<void()> fn);

} // namespace detail

// RAII класс для регистрации функции для бенчмаркинга.
class builder_t {
public:
    template<typename Callable>
    builder_t(std::string ns, std::string cs, Callable fn, bool baseline = false) {
        auto& overlord = overlord_t::instance();
        overlord.add(ns, cs, baseline, detail::wrap(fn));
    }
};

} // namespace ticktack
