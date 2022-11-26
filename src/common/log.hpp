#pragma once

#include <iostream>
#include <string>
#include <Windows.h>

#include "common/timing.hpp"
#include "color-console/include/color.hpp"

namespace logging {

    constexpr unsigned ERROR_STAGE = 0;
    constexpr unsigned WARNING_STAGE = 1;
    constexpr unsigned INFO_STAGE = 2;
    constexpr unsigned DEBUG_STAGE = 3;

    unsigned stage = DEBUG_STAGE;

    void set_stage(unsigned n) {
        stage = n;
    }
    
    void debug(const std::string& str) {
        if (stage >= DEBUG_STAGE)
            std::cout
                << dye::grey("[" + timing::local_time() + "] " + "debug: " + str)
                << std::endl;
    }

    void info(const std::string& str) {
        if (stage >= INFO_STAGE)
            std::cout
                << dye::aqua("[" + timing::local_time() + "] " + "info: " + str)
                << std::endl;
    }

    void warning(const std::string& str) {
        if (stage >= WARNING_STAGE)
            std::cout
                << dye::yellow("[" + timing::local_time() + "] " + "warning: " + str)
                << std::endl;
    }

    void error(const std::string& str) {
        if (stage >= ERROR_STAGE)
            std::cout
                << dye::red("[" + timing::local_time() + "] " + "error: " + str)
                << std::endl;
    }
}

class logging_buff {

private:
    std::string log_buff;
    std::stringstream ss;

public:
    template<class T>
    logging_buff& operator<<(const T& output) {
        ss << output;
        return *this;
    }

    operator const std::string& () {
        return return_and_flush();
    }

    const std::string& str() {
        return return_and_flush();
    }

    const std::string& return_and_flush() {
        log_buff = ss.str();
        ss.str(std::string());
        return log_buff;
    }
};
