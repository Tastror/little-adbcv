#pragma once

#include <chrono>
#include <thread>
#include <string>
#include <iomanip>


namespace timing {

    // local time formatted by std::put_time(%c)
    std::string local_time() {
        std::stringstream str_stream;
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        str_stream << std::put_time(&tm, "%c");
        return str_stream.str();
    }

    // local_time with "-" between numbers
    // e.g. 22-11-20-14-05-12
    std::string local_time_for_file() {
        std::stringstream str_stream;
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        str_stream << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S");
        return str_stream.str();
    }

    // return std::chrono::steady_clock::time_point
    // you can use auto to get it
    auto now() {
        return std::chrono::steady_clock::now();
    }

    // use std::this_thread::sleep_for()
    void milli_sleep(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    // use std::this_thread::sleep_for()
    void seconds_sleep(double seconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds((long long)(seconds * 1000)));
    }

    // use std::this_thread::sleep_until()
    void milli_sleep2(int milliseconds) {
        std::this_thread::sleep_until(now() + std::chrono::milliseconds(milliseconds));
    }

    // use std::this_thread::sleep_until()
    void seconds_sleep2(double seconds) {
        std::this_thread::sleep_until(now() + std::chrono::milliseconds((long long)(seconds * 1000)));
    }
}