#include <iostream>
#include <unistd.h>

#include <boost/lexical_cast.hpp>

#include <epicmeter/benchmark.hpp>
#include <epicmeter/output/json.hpp>

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
    epicmeter::options_t options;
    options.time.min = 1e8;
    options.time.max = 1e9;
    options.iters = epicmeter::iteration_type(1);

    int opt = 0;
    auto& overlord = epicmeter::overlord_t::instance();

    while ((opt = getopt(argc, argv, "n:x:i:h:j")) != EOF) {
        switch (opt) {
        case 'n':
            options.time.min = 1e6 * boost::lexical_cast<epicmeter::nanosecond_type::value_type>(optarg);
            break;
        case 'x':
            options.time.max = 1e6 * boost::lexical_cast<epicmeter::nanosecond_type::value_type>(optarg);
            break;
        case 'i':
            options.iters = epicmeter::iteration_type(
                boost::lexical_cast<epicmeter::iteration_type::value_type>(optarg)
            );
            break;
        case 'j':
            overlord.output(
                std::unique_ptr<epicmeter::output::printer_t>(new epicmeter::output::json_t(std::cerr))
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

    overlord.options(options);
    overlord.run();
    return 0;
}
