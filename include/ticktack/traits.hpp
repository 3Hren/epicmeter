#pragma once

#include <functional>
#include <tuple>
#include <utility>

#include "ticktack/types.hpp"

namespace ticktack {

template<typename T>
struct function_traits;

template<class Class, typename R, typename... Args>
struct function_traits<R(Class::*)(Args...) const> {
    enum { arity = sizeof...(Args) };
    typedef R result_type;
    typedef std::function<R(Args...)> function_type;

    template<size_t i>
    struct arg {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
    enum { arity = sizeof...(Args) };
    typedef R result_type;
    typedef std::function<R(Args...)> function_type;

    template<size_t i>
    struct arg {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template<class Callable>
struct function_traits : public function_traits<decltype(&Callable::operator())> {};

template<typename Callable>
struct accepts_itercount {
    static const bool value =
        !std::is_same<
            iteration_type,
            typename function_traits<Callable>::result_type
        >::value &&
        std::is_same<
            iteration_type,
            typename function_traits<Callable>::template arg<0>::type
        >::value;
};

template<typename Callable>
struct returns_itercount {
    static const bool value =
        std::is_same<
            iteration_type,
            typename function_traits<Callable>::result_type
        >::value &&
        !std::is_same<
            iteration_type,
            typename function_traits<Callable>::template arg<0>::type
        >::value;
};

} // namespace ticktack
