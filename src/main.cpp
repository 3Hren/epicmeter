#include <iostream>
#include <unistd.h>

#include <boost/lexical_cast.hpp>

#include <ticktack/benchmark.hpp>
#include <ticktack/printer/json.hpp>

void help(char** argv) {
    std::cout << "Usage: " << argv[0] << " [-nxih]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << " n \t minimum time per benchmark, ms (default: 100)" << std::endl;
    std::cout << " x \t maximum time per benchmark, ms (default: 1000)" << std::endl;
    std::cout << " i \t iteration count per benchmark  (default: 1)" << std::endl;
    std::cout << " j \t json output" << std::endl;
}

int main(int argc, char** argv) {
    ticktack::options_t options;
    options.time.min = 1e8;
    options.time.max = 1e9;
    options.iters = ticktack::iteration_type(1);

    int opt = 0;
    auto& benchmark = ticktack::benchmarker_t::instance();

    while ((opt = getopt(argc, argv, "n:x:i:h:j")) != EOF) {
        switch (opt) {
        case 'n':
            options.time.min = 1e6 * boost::lexical_cast<ticktack::nanosecond_type>(optarg);
            break;
        case 'x':
            options.time.max = 1e6 * boost::lexical_cast<ticktack::nanosecond_type>(optarg);
            break;
        case 'i':
            options.iters = ticktack::iteration_type(
                boost::lexical_cast<ticktack::iteration_type::value_type>(optarg)
            );
            break;
        case 'j':
            benchmark.set_watcher(
                std::unique_ptr<ticktack::watcher_t>(new ticktack::json_printer_t())
            );
            break;
        case 'h':
            help(argv);
            exit(0);
            break;
        case '?':
            help(argv);
            exit(1);
            break;
        }
    }

    benchmark.set_options(options);
    benchmark.run();
    return 0;
}

//!@todo: Add +- fields.
