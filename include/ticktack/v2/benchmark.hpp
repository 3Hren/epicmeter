#pragma once

#include <array>
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>

#include "ticktack/v2/types.hpp"
#include "ticktack/v2/traits.hpp"
#include "ticktack/v2/macro.hpp"
#include "ticktack/v2/output/mod.hpp"
#include "ticktack/compiler.hpp"

namespace boost { template<typename> class optional; }

namespace ticktack {

struct options_t {
    struct {
        nanosecond_type min;
        nanosecond_type max;
    } time;

    iteration_type iters;
};

class benchmark_t {
public:
    bool baseline;
    std::string description;
    std::function<iteration_type(iteration_type)> fn;
};

class namespace_t {
public:
    std::vector<benchmark_t> benchmarks;
};

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

std::function<iteration_type(iteration_type)>
wrap(std::function<iteration_type()> fn);

std::function<iteration_type(iteration_type)>
wrap(std::function<void(iteration_type)> fn);

std::function<iteration_type(iteration_type)>
wrap(std::function<void()> fn);

} // namespace detail

class builder_t {
public:
    template<typename Callable>
    builder_t(std::string ns, std::string cs, Callable fn, bool baseline = false) {
        auto& overlord = overlord_t::instance();
        overlord.add(ns, cs, baseline, detail::wrap(fn));
    }
};

namespace detail {

template<typename Callable, class = void>
struct param_helper_t;

template<typename Callable>
struct param_helper_t<
    Callable,
    typename std::enable_if<
        !accepts_itercount<Callable>::value && !returns_itercount<Callable>::value
    >::type
> {
    template<typename... Args>
    static void create(std::string ns, std::string cs, Callable fn, Args&&... a) {
        std::function<void()> bound = std::bind(std::move(fn), std::forward<Args>(a)...);
        overlord_t::instance().add(ns,cs, false, detail::wrap(std::move(bound)));
    }
};

template<typename Callable>
struct param_helper_t<
    Callable,
    typename std::enable_if<returns_itercount<Callable>::value>::type
> {
    template<typename... Args>
    static void create(std::string ns, std::string cs, Callable fn, Args&&... a) {
        std::function<iteration_type()> bound = std::bind(std::move(fn), std::forward<Args>(a)...);
        overlord_t::instance().add(ns, cs, false, detail::wrap(std::move(bound)));
    }
};

template<typename Callable>
struct param_helper_t<
    Callable,
    typename std::enable_if<accepts_itercount<Callable>::value>::type
> {
    template<typename... Args>
    static void create(std::string ns, std::string cs, Callable fn, Args&&... a) {
        std::function<void(iteration_type)> bound = std::bind(
            std::move(fn),
            std::placeholders::_1,
            std::forward<Args>(a)...
        );

        overlord_t::instance().add(ns, cs, false, detail::wrap(std::move(bound)));
    }
};

struct bind_t {
    template<typename Callable, typename... Args>
    bind_t(std::string suite, std::string name, Callable fn, Args&&... a) {
        param_helper_t<Callable>::create(suite, name, fn, std::forward<Args>(a)...);
    }
};

} // namespace detail

} // namespace ticktack
