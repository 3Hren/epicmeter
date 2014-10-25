#pragma once

#include <boost/preprocessor/seq/cat.hpp>

#define TT_THIRD(a, b, ...) __VA_ARGS__
#define TT_ONE_OR_NONE(a, ...) TT_THIRD(a, ##__VA_ARGS__, a)

#define TT_FUNCTOR_CLASSNAME(suite) BOOST_PP_SEQ_CAT((TT_REGISTRATOR_NAME(suite))(_t))
#define TT_ITERTYPE ::ticktack::iteration_type

#define TT_REGISTRATOR_TYPE ::ticktack::builder_t
#define TT_REGISTRATOR_NAME(suite) BOOST_PP_SEQ_CAT((suite)(__LINE__))

#define TT_REGISTRATOR(baseline, ns, cs, R, PT, PN, ...) \
    struct TT_FUNCTOR_CLASSNAME(ns) { R operator()(PT PN) const; }; \
    static TT_REGISTRATOR_TYPE TT_REGISTRATOR_NAME(ns)(#ns, #cs, TT_FUNCTOR_CLASSNAME(ns)(), baseline); \
    R TT_FUNCTOR_CLASSNAME(ns)::operator()(PT PN) const

#define BENCHMARK(ns, cs, ...) \
    TT_REGISTRATOR(false, ns, cs, void, TT_ONE_OR_NONE(TT_ITERTYPE, ##__VA_ARGS__), __VA_ARGS__)

#define BENCHMARK_BASELINE(ns, cs, ...) \
    TT_REGISTRATOR(true,  ns, cs, void, TT_ONE_OR_NONE(TT_ITERTYPE, ##__VA_ARGS__), __VA_ARGS__)

#define BENCHMARK_RETURN(ns, cs, ...) \
    TT_REGISTRATOR(false, ns, cs, TT_ITERTYPE, TT_ONE_OR_NONE(TT_ITERTYPE, ##__VA_ARGS__), __VA_ARGS__)

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

#define TT_BIND_TYPE ::ticktack::detail::bind_t
#define BENCHMARK_BOUND(ns, fn, ...) \
    static TT_BIND_TYPE TT_ANONYMOUS_VARIABLE(ns##fn)(#ns, #fn"("#__VA_ARGS__")", fn, __VA_ARGS__)
