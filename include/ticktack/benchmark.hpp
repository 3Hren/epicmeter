#pragma once

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

#include <boost/preprocessor.hpp>

#include "ticktack/compiler.hpp"
#include "ticktack/traits.hpp"
#include "ticktack/types.hpp"
#include "ticktack/watcher.hpp"

namespace ticktack {

namespace detail {

inline
runner::measurer_t
wrap(runner::measurer_t fn) {
    return fn;
}

inline
runner::measurer_t
wrap(std::function<iteration_type()> fn) {
    return runner::measurer_t { runner::repeater_t { std::move(fn) } };
}

inline
runner::measurer_t
wrap(std::function<void(iteration_type)> fn) {
    return runner::measurer_t { runner::pass_t { std::move(fn) } };
}

inline
runner::measurer_t
wrap(std::function<void()> fn) {
    return runner::measurer_t { runner::repeater_t { runner::single_t { std::move(fn) } } };
}

} // namespace detail

namespace registrator {

struct benchmark_t {
    template<typename R, typename... Args>
    benchmark_t(const char* suite, const char* name, R(*fn)(Args...)) {
        auto& benchmarker = benchmarker_t::instance();
        benchmarker.add(
            suite,
            name,
            detail::wrap(std::function<R(Args...)>(fn))
        );
    }

    template<typename Callable>
    benchmark_t(const char* suite, const char* name, Callable fn) {
        auto& benchmarker = benchmarker_t::instance();
        benchmarker.add(
            suite,
            name,
            detail::wrap(typename function_traits<Callable>::function_type(fn))
        );
    }
};

struct baseline_t {
    template<typename R, typename... Args>
    baseline_t(const char* suite, const char* name, R(*fn)(Args...)) {
        auto& benchmarker = benchmarker_t::instance();
        benchmarker.add_baseline(
            suite,
            name,
            detail::wrap(std::function<R(Args...)>(fn))
        );
    }

    template<typename Callable>
    baseline_t(const char* suite, const char* name, Callable fn) {
        auto& benchmarker = benchmarker_t::instance();
        benchmarker.add_baseline(
            suite,
            name,
            detail::wrap(typename function_traits<Callable>::function_type(fn))
        );
    }
};

struct relative_t {
    template<typename R, typename... Args>
    relative_t(const char* suite, const char* name, R(*fn)(Args...)) {
        auto& benchmarker = benchmarker_t::instance();
        benchmarker.add_relative(
            suite,
            name,
            detail::wrap(std::function<R(Args...)>(fn))
        );
    }

    template<typename Callable>
    relative_t(const char* suite, const char* name, Callable fn) {
        auto& benchmarker = benchmarker_t::instance();
        benchmarker.add_relative(
            suite,
            name,
            detail::wrap(typename function_traits<Callable>::function_type(fn))
        );
    }
};

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
    static void create(std::string suite, std::string name, Callable fn, Args&&... a) {
        std::function<void()> bound = std::bind(
            std::move(fn),
            std::forward<Args>(a)...
        );

        benchmarker_t::instance().add(
            suite,
            name,
            detail::wrap(std::move(bound))
        );
    }
};

template<typename Callable>
struct param_helper_t<
    Callable,
    typename std::enable_if<returns_itercount<Callable>::value>::type
> {
    template<typename... Args>
    static void create(std::string suite, std::string name, Callable fn, Args&&... a) {
        std::function<iteration_type()> bound = std::bind(
            std::move(fn),
            std::forward<Args>(a)...
        );

        benchmarker_t::instance().add(
            suite,
            name,
            detail::wrap(std::move(bound))
        );
    }
};

template<typename Callable>
struct param_helper_t<
    Callable,
    typename std::enable_if<accepts_itercount<Callable>::value>::type
> {
    template<typename... Args>
    static void create(std::string suite, std::string name, Callable fn, Args&&... a) {
        std::function<void(iteration_type)> bound = std::bind(
            std::move(fn),
            std::placeholders::_1,
            std::forward<Args>(a)...
        );

        benchmarker_t::instance().add(
            suite,
            name,
            detail::wrap(std::move(bound))
        );
    }
};

struct benchmark_bind_t {
    template<typename Callable, typename... Args>
    benchmark_bind_t(std::string suite, std::string name, Callable fn, Args&&... a) {
        param_helper_t<Callable>::create(suite, name, fn, std::forward<Args>(a)...);
    }
};

} // namespace registrator

} // namespace ticktack
