#include "common/stream.hpp"
#include "common/log.hpp"
#include "common/format.hpp"
#include "common/timing.hpp"
#include "control/adb.hpp"
#include "control/miniadb.hpp"
#include "vision/vision.hpp"
#include "vision/detect.hpp"
#include "util/util.hpp"

#include "example.hpp"

int main(int argc, char** argv) {

    logging::set_stage(logging::DEBUG_STAGE);

    logging::info("welcome to use little_adbcv");
    logging::warning("this is an warning");

    // adb::kill_server();
    // timing::seconds_sleep(3);

    adb::start_server();
    timing::seconds_sleep(3);
    
    miniadb::init();
    getchar();
    
    example::run();

    return 0;
}