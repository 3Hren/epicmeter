#pragma once

#include <string>

namespace ticktack {

class suite_t;
class analyzed_t;
class watcher_t {
public:
    virtual
    ~watcher_t() {}

    virtual
    void
    set_up() = 0;

    virtual
    void
    suite_start(const std::string& name, const suite_t& suite) = 0;

    virtual
    void
    relative_pass(const std::string& name, const analyzed_t& info_b, const analyzed_t& info) = 0;

    virtual
    void
    pass(const std::string& name, const analyzed_t& info) = 0;

    virtual
    void
    suite_complete(const std::string& suite) = 0;
};

}
