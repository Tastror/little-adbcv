#pragma once

#include <string>
#include <iostream>
#include <tuple>

#include "common/format.hpp"
#include "common/log.hpp"
#include "control/cmd.hpp"

class adb {

private:

    const static std::string adb_prefix;
    const static std::string device_prefix;
    const static std::string connect_prefix;
    const static std::string disconnect_prefix;
    const static std::string screencap_prefix;
    const static std::string tap_prefix;

    const static std::string start_server_order;
    const static std::string kill_server_order;
    const static std::string devices_order;

    static std::string device_name;
    static std::string log_str_buff;
    static std::string res_str_buff;

public:

    std::string get_log_str_buff() const {
        return log_str_buff;
    }

    std::string get_res_str_buff() const {
        return res_str_buff;
    }

    std::string get_device_name() const {
        return device_name;
    }

    static void set_device_name(const std::string& name) {
        if (name == "")
            device_name = "emulator-5554";
        else
            device_name = name;
        
        log_str_buff = "<adb> device name is set to " + device_name;
        logging::debug(log_str_buff);
    }

    static std::string connect(int port) {

        std::string order =
            adb_prefix +
            connect_prefix + std::to_string(port);
        
        log_str_buff = "<adb> connect to " + std::to_string(port);
        logging::debug(log_str_buff);

        res_str_buff = cmd::cmd_popen(order);
        return res_str_buff;
    }

    static std::string disconnect(int port) {
        std::string order =
            adb_prefix +
            disconnect_prefix + std::to_string(port);
        
        log_str_buff = "<adb> disconnect to " + std::to_string(port);
        logging::debug(log_str_buff);

        res_str_buff = cmd::cmd_popen(order);
        return res_str_buff;
    }

    static std::string start_server() {
        std::string order =
            adb_prefix + start_server_order;

        log_str_buff = "<adb> start server";
        logging::debug(log_str_buff);

        res_str_buff = cmd::cmd_popen(order);
        return res_str_buff;
    }

    static std::string kill_server() {
        std::string order =
            adb_prefix + kill_server_order;

        log_str_buff = "<adb> kill server";
        logging::debug(log_str_buff);

        res_str_buff = cmd::cmd_popen(order);
        return res_str_buff;
    }

    static std::string devices() {
        std::string order =
            adb_prefix + devices_order;

        log_str_buff = "<adb> show devices";
        logging::debug(log_str_buff);

        res_str_buff = cmd::cmd_popen(order);
        return res_str_buff;
    } 

    static std::string cap(const std::string& save_path) {
        std::string order =
            adb_prefix +
            device_prefix + device_name + " " +
            screencap_prefix + format::quote(save_path);

        log_str_buff = "<adb> capture to " + format::quote(save_path);
        logging::debug(log_str_buff);

        res_str_buff = cmd::cmd_popen(order);
        return res_str_buff;
    }

    // tap tuple(x, y), left-up is (0, 0)
    static std::string tap(std::tuple<int, int> xy) {
        return tap(std::get<0>(xy), std::get<1>(xy));
    }

    // tap (x, y), left-up is (0, 0)
    static std::string tap(int x, int y) {
        std::string order =
            adb_prefix +
            device_prefix + device_name + " " +
            tap_prefix + std::to_string(x) + " " + std::to_string(y);

        log_str_buff = "<adb> tap in (" + std::to_string(x) + ", " + std::to_string(y) + ")";
        logging::debug(log_str_buff);

        res_str_buff = cmd::cmd_popen(order);
        return res_str_buff;
    }

};

const std::string adb::adb_prefix = "adb ";
const std::string adb::device_prefix = "-s ";
const std::string adb::connect_prefix = "connect ";
const std::string adb::disconnect_prefix = "disconnect ";
const std::string adb::screencap_prefix = "exec-out screencap -p > ";
const std::string adb::tap_prefix = "shell input tap ";

const std::string adb::start_server_order = "start-server";
const std::string adb::kill_server_order = "kill-server";
const std::string adb::devices_order = "devices";

std::string adb::device_name = "emulator-5554";
std::string adb::log_str_buff = "";
std::string adb::res_str_buff = "";