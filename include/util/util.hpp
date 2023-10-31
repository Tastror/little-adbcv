#pragma once

#include "common/format.hpp"
#include "common/log.hpp"
#include "control/adb.hpp"
#include "vision/vision.hpp"
#include "vision/detect.hpp"

// make vision::, detect:: and adb:: easy to use
// it will save many path name, in order to avoid redundant input
class util {

private:
    static std::string screen_path;
    static std::string reference_dir, reference_postfix;
    static logging_buff log_buff;

public:
    static bool last_wait_success;

    // full path use for screen capture's saving place
    static void set_screen_path(const std::string& path) {
        screen_path = path;
    }

    // string use for vision::read_ref()'s prefix,
    // thus you can use util::read_ref_name() instead of vision::read_ref()
    // where you do not need to specify the directory prefix
    // e.g. "a\\b/c\\d" or "a\\b/c\\d\\" are both OK, will transfered into "a\\b\\c\\d\\"
    static void set_read_ref_prefix(const std::string& reference_dir_path) {
        reference_dir = format::prefix_right_slash(reference_dir_path);
    }

    // string use for vision::read_ref()'s postfix,
    // thus you can use util::read_ref_name() instead of vision::read_ref(),
    // where you do not need to specify the directory postfix
    // e.g., "png" or ".png" are both OK, will transfered into ".png"
    static void set_read_ref_postfix(const std::string& reference_postfix_name) {
        reference_postfix = format::postfix_point(reference_postfix_name);
    }

    // [adb] shot a screen picutre, save it to path, and the [vision] read it
    // default saving path is screen_path, you are able to specify it
    static void adb_cap_vision_save(const std::string& path = screen_path) {
        adb::cap(path);
        vision::read_screen(path);
    }

    // wait until reference picture matches current screen
    // please use `vision::read_ref()` or `util::read_ref_name()` before this
    // in order to make the [vision] get the right reference picture
    // percentage means which subscreen to match (only match the picture in subscreen)
    // result saved in `util::last_wait_success`
    static bool wait_cap(
        const std::tuple<double, double, double, double>& percentage = vision::FULL_SCREEN,
        int max_try_time = 20, double wait_second = 0.5,
        const std::string& path = screen_path
    ) {
        int count = max_try_time;
        do {
            log_buff << "<util> remain count = " << count;
            logging::debug(log_buff);
            if (count <= 0) {
                last_wait_success = false;
                return false;
            }
            util::adb_cap_vision_save(path);
            vision::cut_sub_screen_percentage(percentage);
            if (vision::match()) {
                last_wait_success = true;
                return true;
            }
            timing::seconds_sleep(wait_second);
            count--;
        } while (true);
    }

    // vision::read_ref() without prefix or postfix
    // use vision::read_ref(reference_dir + only_file_name + reference_postfix)
    // spaces, dots, slashes allowed, but no "\\" or "/" at the beginning or end of only_file_name
    static const cv::Mat& read_ref_name(const std::string& only_file_name) {
        return vision::read_ref(reference_dir + only_file_name + reference_postfix);
    }

};

bool util::last_wait_success;
std::string util::screen_path = "C:\\temp.png";
std::string util::reference_dir, util::reference_postfix;

logging_buff util::log_buff;