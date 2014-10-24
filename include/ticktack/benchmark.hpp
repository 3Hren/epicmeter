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

struct options_t {
    struct {
        nanosecond_type min;
        nanosecond_type max;
    } time;

    iteration_type iters;
};

struct result_t {
    nanosecond_type total;
    iteration_type iters;
};

struct analyzed_t {
    double min;
    double mean;
    double stddev;
};

namespace runner {

struct single_t {
    std::function<void()> fn;
    iteration_type operator()() const;
};

struct pass_t {
    std::function<void(iteration_type)> fn;
    iteration_type operator()(iteration_type times) const;
};

struct repeater_t {
    std::function<iteration_type()> fn;
    iteration_type operator()(iteration_type times) const;
};

struct measurer_t {
    std::function<iteration_type(iteration_type)> fn;
    result_t operator()(iteration_type times) const;
};

} // namespace runner

struct runnable_t {
    std::string name;
    runner::measurer_t fn;
};

struct suite_t {
    bool batch;
    std::vector<runnable_t> benchmarks;

    suite_t() : batch(false) {}
};

class benchmarker_t {
private:
    options_t options;
    std::unordered_map<std::string, suite_t> suites;
    std::unique_ptr<watcher_t> watcher;

public:
    static benchmarker_t& instance();

    void add_baseline(std::string suite, std::string name, runner::measurer_t function);
    void add_relative(std::string suite, std::string name, runner::measurer_t function);
    void add(std::string suite, std::string name, runner::measurer_t function);

    void set_options(options_t options);
    void set_watcher(std::unique_ptr<watcher_t> watcher);

    void run();
    void run(suite_t& suite);
    std::vector<result_t> run(const runner::measurer_t& fn) const;

private:
    benchmarker_t();
};

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

#define TT_THIRD(a, b, ...) __VA_ARGS__
#define TT_ONE_OR_NONE(a, ...) TT_THIRD(a, ##__VA_ARGS__, a)

#define TT_FUNCTOR_CLASSNAME(__suite__, __benchname__) \
    BOOST_PP_SEQ_CAT((__suite__)(__LINE__)(_t))

#define TT_REGISTRATOR_NAME(__suite__, __benchname__) \
    BOOST_PP_SEQ_CAT((__suite__)(__LINE__))

#define TT_REGISTRATOR(__target__, __suite__, __benchname__, __return_type__, __param_type__, __param_name__, ...) \
    struct TT_FUNCTOR_CLASSNAME(__suite__, __benchname__) { \
        __return_type__ operator()(__param_type__ __param_name__) const; \
    }; \
    static ::ticktack::registrator::__target__ TT_REGISTRATOR_NAME(__suite__, __benchname__)( \
        #__suite__, \
        #__benchname__, \
        TT_FUNCTOR_CLASSNAME(__suite__, __benchname__)() \
    ); \
    __return_type__ TT_FUNCTOR_CLASSNAME(__suite__, __benchname__)::operator()(__param_type__ __param_name__) const

#define BENCHMARK(__suite__, __benchname__, ...) \
    TT_REGISTRATOR( \
        benchmark_t, \
        __suite__, \
        __benchname__, \
        void, \
        TT_ONE_OR_NONE(iteration_type, ##__VA_ARGS__), \
        __VA_ARGS__ \
    )

#define BENCHMARK_BASELINE(__suite__, __benchname__, ...) \
    TT_REGISTRATOR( \
        baseline_t, \
        __suite__, \
        __benchname__, \
        void, \
        TT_ONE_OR_NONE(::ticktack::iteration_type, ##__VA_ARGS__), \
        __VA_ARGS__ \
    )

#define BENCHMARK_RELATIVE(__suite__, __benchname__, ...) \
    TT_REGISTRATOR( \
        relative_t, \
        __suite__, \
        __benchname__, \
        void, \
        TT_ONE_OR_NONE(iteration_type, ##__VA_ARGS__), \
        __VA_ARGS__ \
    )

#define BENCHMARK_RETURN(__suite__, __benchname__, ...) \
    TT_REGISTRATOR( \
        benchmark_t, \
        __suite__, \
        __benchname__, \
        ::ticktack::iteration_type, \
        TT_ONE_OR_NONE(::ticktack::iteration_type, ##__VA_ARGS__), \
        __VA_ARGS__ \
    )

#ifndef TT_ANONYMOUS_VARIABLE
#   define TT_CONCATENATE_IMPL(x, y) x##y
#   define TT_CONCATENATE(x, y) TT_CONCATENATE_IMPL(x, y)
#   ifdef __COUNTER__
#       define TT_ANONYMOUS_VARIABLE(str) TT_CONCATENATE(str, __COUNTER__)
#   else
#       define TT_ANONYMOUS_VARIABLE(str) TT_CONCATENATE(str, __LINE__)
#   endif
#endif
#define TT_STRINGIZE(x) #x

#define BENCHMARK_BOUND(__suite__, __fn__, ...) \
    static ::ticktack::registrator::benchmark_bind_t TT_ANONYMOUS_VARIABLE(__suite__##__fn__)( \
        #__suite__, \
        #__fn__"("#__VA_ARGS__")", \
        __fn__, \
        __VA_ARGS__ \
    )
