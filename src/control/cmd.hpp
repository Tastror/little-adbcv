#pragma once

#include <string>
#include <stdio.h>
#include <direct.h>

#include "common/log.hpp"

namespace cmd {

    std::string get_current_path() {
        char *current_path_c_str;
        std::string current_path;
        if ((current_path_c_str = getcwd(NULL, 0)) == NULL) {
            logging::error("<cmd> getcwd failed");
        }
        else {
            current_path = std::string(current_path_c_str);
            free(current_path_c_str);
        }
        return current_path;
    }

    std::string cmd_popen(const std::string& cmdLine) {
        char buffer[1024] = { '\0' };
        FILE* pf = NULL;
        pf = _popen(cmdLine.c_str(), "r");
        if (NULL == pf) {
            logging::error("<cmd> open pipe failed");
            return std::string("");
        }
        std::string ret;
        while (fgets(buffer, sizeof(buffer), pf)) {
            ret += buffer;
        }
        _pclose(pf);
        return ret;
    }

} // namespace cmd

