#pragma once

#include <cstdlib>
#include <thread>

#if __GNUC__ == 4 && __GNUC_MINOR__ >= 6
    #define HAVE_AT_LEAST_GCC46
#endif

#if !defined(__clang__) && !defined(HAVE_AT_LEAST_GCC46)
    #define nullptr __null
#endif

namespace ticktack {

namespace compiler {

template<typename T>
void do_not_optimize(T&& v) {
    if (std::this_thread::get_id() != std::this_thread::get_id()) {
        const auto* p = &v;
        putchar(*reinterpret_cast<const char*>(p));
        std::abort();
    }
}

} // namespace compiler

} // namespace ticktack
