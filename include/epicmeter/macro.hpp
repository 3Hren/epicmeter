#pragma once

#include <boost/preprocessor/seq/cat.hpp>

#define TT_THIRD(a, b, ...) __VA_ARGS__
#define TT_ONE_OR_NONE(a, ...) TT_THIRD(a, ##__VA_ARGS__, a)

#define TT_FUNCTOR_CLASSNAME(reg) BOOST_PP_SEQ_CAT((reg)(_t))
#define TT_ITERTYPE ::epicmeter::iteration_type

#define TT_REGISTRATOR_TYPE ::epicmeter::builder_t

#define TT_REGISTRATOR(baseline, reg, ns, cs, R, PT, PN, ...) \
    struct TT_FUNCTOR_CLASSNAME(reg) { R operator()(PT PN) const; }; \
    static TT_REGISTRATOR_TYPE reg(#ns, #cs, TT_FUNCTOR_CLASSNAME(reg)(), baseline); \
    R TT_FUNCTOR_CLASSNAME(reg)::operator()(PT PN) const

#define BENCHMARK(ns, cs, ...) \
    TT_REGISTRATOR(false, TT_ANONYMOUS_VARIABLE(ns), ns, cs, void, TT_ONE_OR_NONE(TT_ITERTYPE, ##__VA_ARGS__), __VA_ARGS__)

#define BENCHMARK_BASELINE(ns, cs, ...) \
    TT_REGISTRATOR(true, TT_ANONYMOUS_VARIABLE(ns), ns, cs, void, TT_ONE_OR_NONE(TT_ITERTYPE, ##__VA_ARGS__), __VA_ARGS__)

#define BENCHMARK_RETURN(ns, cs, ...) \
    TT_REGISTRATOR(false, TT_ANONYMOUS_VARIABLE(ns), ns, cs, TT_ITERTYPE, TT_ONE_OR_NONE(TT_ITERTYPE, ##__VA_ARGS__), __VA_ARGS__)

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

#define TT_BIND_TYPE ::epicmeter::detail::bind_t
#define BENCHMARK_BOUND(ns, fn, ...) \
    static TT_BIND_TYPE TT_ANONYMOUS_VARIABLE(ns##fn)(#ns, #fn"("#__VA_ARGS__")", fn, __VA_ARGS__)
