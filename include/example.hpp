#pragma once

#include "common/stream.hpp"
#include "common/log.hpp"
#include "common/format.hpp"
#include "common/timing.hpp"
#include "control/adb.hpp"
#include "control/miniadb.hpp"
#include "vision/vision.hpp"
#include "vision/detect.hpp"
#include "util/util.hpp"

namespace example {

    void run() {
        
        const std::string current_path = format::prefix_right_slash(cmd::get_current_path());

        const std::string screen_save_path = current_path + "temp.png";
        const std::string ref_img_dir = current_path + "img";

        const double wait_time = 0.5;

        std::tuple<double, double> tap_point = std::make_tuple(0.5, 0.8);

        util::set_screen_path(screen_save_path);
        util::set_read_ref_prefix(ref_img_dir);
        util::set_read_ref_postfix(".png");

        logging::warning("this is an example");
        logging::warning("log stage is " + std::to_string(logging::stage));


        // example 1.1
        // use no util

        adb::cap(screen_save_path);
        vision::read_screen(screen_save_path);
        vision::read_ref(
            format::prefix_right_slash(ref_img_dir)
            + "example"
            + format::postfix_point(".png")
        );
        if (vision::match()) logging::warning("example 1.1 matched");
        else logging::warning("example 1.1 did not match");
        // adb::tap(vision::get_match_shift());
        

        // example 2.1
        // use util::adb_cap_vision_save() and util::read_ref_name()

        util::adb_cap_vision_save();
        util::read_ref_name("example");
        if (vision::match()) logging::warning("example 2.1 matched");
        else logging::warning("example 2.1 did not match");
        // adb::tap(vision::get_match_shift());


        // example 2.2
        // use util::adb_cap_vision_save() and util::read_ref_name()
        // use vision::cut_sub_screen_percentage()

        util::adb_cap_vision_save();
        util::read_ref_name("example");
        vision::cut_sub_screen_percentage(vision::RIGHT_DOWN_SCREEN);
        if (vision::match()) logging::warning("example 2.2 matched");
        else logging::warning("example 2.2 did not match");
        // adb::tap(vision::get_match_shift());


        // example 3.1
        // use util::read_ref_name() and util::wait_cap()

        util::read_ref_name("example");
        util::wait_cap(vision::LEFT_UP_SCREEN, 5);
        if (util::last_wait_success) logging::warning("example 3.1 matched");
        else logging::warning("example 3.1 did not matched");
        adb::tap(vision::get_match_shift());

        return;
    }

} 
