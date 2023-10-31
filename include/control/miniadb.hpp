#pragma once

#include <string>
#include <iostream>
#include <tuple>

#include "common/format.hpp"
#include "common/log.hpp"
#include "control/cmd.hpp"

class miniadb {

private:

    const static std::string minipath;
    const static std::string minipath_adb_dir, minipath_adb;
    const static std::string minitouch_cmd;
    static std::string minipath_abs, ABI;

    static logging_buff log_buff;

public:

    // check the adb architecture
    static void init() {
        cmd::cmd_popen(R"(
            for /f %i in ('adb shell getprop ro.product.cpu.abi') do set $ABI=%i
        )");
        cmd::cmd_popen(R"(
            set $ABI=%$ABI:\r=%
        )");
        ABI = cmd::cmd_popen(R"(
            echo %$ABI%
        )");
        ABI.pop_back();
        log_buff << "ABI = " << ABI;
        logging::debug(log_buff);

        minipath_abs = format::prefix_right_slash(cmd::get_current_path()) + minipath;
        minipath_abs = cmd::cmd_popen("echo \"" + minipath_abs + "\"");
        minipath_abs.pop_back();
        log_buff << "minipath = " << minipath_abs;
        logging::debug(log_buff);

        cmd::cmd_popen(
            "adb push " + format::quote(minipath_abs) + " " + format::quote(minipath_adb_dir)
        );

        log_buff << cmd::cmd_popen(minitouch_cmd + "-h");
        logging::debug(log_buff);
    }

};

const std::string miniadb::minipath = "src\\minitouch\\libs\\%$ABI%\\minitouch";
std::string miniadb::minipath_abs = "";
const std::string miniadb::minipath_adb_dir = "/data/local/tmp/";
const std::string miniadb::minipath_adb = minipath_adb_dir + "minitouch";
const std::string miniadb::minitouch_cmd = "adb shell " + minipath_adb + " ";
std::string miniadb::ABI = "";

logging_buff miniadb::log_buff;