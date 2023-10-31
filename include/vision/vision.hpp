#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <tuple>
#include <set>

#include "common/stream.hpp"
#include "common/format.hpp"
#include "common/log.hpp"


namespace shift {
    
    const int X_EXTREME_LEFT = 0x10;
    const int X_LEFT = 0x40;
    const int X_CENTER = 0x80;
    const int X_RIGHT = 0xC0;
    const int X_EXTREME_RIGHT = 0xF0;

    const int Y_EXTREME_UP = 0x01;
    const int Y_UP = 0x04;
    const int Y_CENTER = 0x08;
    const int Y_DOWN = 0x0C;
    const int Y_EXTREME_DOWN = 0x0F;
}

class vision {

private:

    static cv::Mat screen_buff, sub_screen_buff, ref_buff;
    // (x, y) or (cols, rows)
    static std::tuple<int, int> screen_shape, ref_shape;
    // (left-up x, left-up y, right-down x, right-down y)
    static std::tuple<int, int, int, int> sub_screen_scale, match_scale;

    static logging_buff log_buff;

public:

    static constexpr auto FULL_SCREEN = std::make_tuple(0, 0, 1, 1);
    static constexpr auto CENTER_SCREEN = std::make_tuple(0.25, 0.25, 0.75, 0.75);
    static constexpr auto LEFT_UP_SCREEN = std::make_tuple(0, 0, 0.5, 0.5);
    static constexpr auto RIGHT_UP_SCREEN = std::make_tuple(0.5, 0, 1, 0.5);
    static constexpr auto LEFT_DOWN_SCREEN = std::make_tuple(0, 0.5, 0.5, 1);
    static constexpr auto RIGHT_DOWN_SCREEN = std::make_tuple(0.5, 0.5, 1, 1);

    static auto get_screen_shape() {
        return screen_shape;
    }

    static auto get_ref_shape() {
        return ref_shape;
    }

    static auto get_sub_screen_scale() {
        return sub_screen_scale;
    }

    static auto get_match_scale() {
        return match_scale;
    }

    // INPUT: full path of the screen picture
    // RETURN: screen_buff
    // USAGE: LITERALLY
    // HINT: once read, the sub_screen_buff will be the same as screen_buff
    static const cv::Mat& read_screen(const std::string& path) {
        screen_buff = cv::imread(path);
        screen_shape = std::make_tuple(screen_buff.cols, screen_buff.rows);

        log_buff << "<vision> reading screen " << format::quote(path) << " of " << screen_shape;
        logging::debug(log_buff);

        // do not need to use cut_sub_screen_percentage(0, 0, 1, 1), just this
        sub_screen_scale = std::make_tuple(0, 0, screen_buff.cols, screen_buff.rows);
        sub_screen_buff = screen_buff;

        return screen_buff;
    }

    // INPUT: percentage [0.0, 1.0] * 4 in screen_buff
    // RETURN: sub_screen_buff
    // USAGE: LITERALLY
    static const cv::Mat& cut_sub_screen_percentage(
        double left_up_x_percentage,
        double left_up_y_percentage,
        double right_down_x_percentage,
        double right_down_y_percentage
    ) {
        return cut_sub_screen(std::make_tuple(
            (int)(std::get<0>(screen_shape) * left_up_x_percentage),
            (int)(std::get<1>(screen_shape) * left_up_y_percentage),
            (int)(std::get<0>(screen_shape) * right_down_x_percentage),
            (int)(std::get<1>(screen_shape) * right_down_y_percentage)
        ));
    }

    // INPUT: percentage [0.0, 1.0] * 4 in screen_buff
    // RETURN: sub_screen_buff
    // USAGE: LITERALLY
    static const cv::Mat& cut_sub_screen_percentage(
        const std::tuple<double, double, double, double>& percentage
    ) {
        return cut_sub_screen(std::make_tuple(
            (int)(std::get<0>(screen_shape) * std::get<0>(percentage)),
            (int)(std::get<1>(screen_shape) * std::get<1>(percentage)),
            (int)(std::get<0>(screen_shape) * std::get<2>(percentage)),
            (int)(std::get<1>(screen_shape) * std::get<3>(percentage))
        ));
    }

    // INPUT: (left-up x, left-up y, right-down x, right-down y) in screen_buff
    // RETURN: sub_screen_buff
    // USAGE: LITERALLY
    // HINT: recommend to use `cut_sub_screen_percentage()`
    static const cv::Mat& cut_sub_screen(std::tuple<int, int, int, int> sub_scale) {

        sub_screen_scale = sub_scale;
        int x, y, z, w;
        std::tie(x, y, z, w) = sub_screen_scale;
        cv::Rect sub_rect(x, y, z - x, w - y);
        sub_screen_buff = screen_buff(sub_rect);

        log_buff << "<vision> sub screen scale = " << sub_screen_scale;
        logging::debug(log_buff);

        // debug
        // cv::imshow("sub_screen", sub_screen_buff);
        // cv::waitKey(0);
        // end debug

        return sub_screen_buff;
    }

    // INPUT: full path of the reference picture
    // RETURN: ref_buff
    // USAGE: LITERALLY
    static const cv::Mat& read_ref(const std::string& path) {
        ref_buff = cv::imread(path);
        ref_shape = std::make_tuple(ref_buff.cols, ref_buff.rows);

        log_buff << "<vision> reading reference " << format::quote(path) << " of " << ref_shape;
        logging::debug(log_buff);

        return ref_buff;
    }

    // INPUT: percentage [0.0, 1.0] * 2 in screen_buff
    // RETURN: point of that percentage in screen_buff
    // USAGE: LITERALLY
    static auto get_screen_percentage(const std::tuple<double, double>& percentage) -> decltype(screen_shape) {
        return get_screen_percentage(std::get<0>(percentage), std::get<1>(percentage));
    }

    // INPUT: percentage [0.0, 1.0] * 2 in screen_buff
    // RETURN: point of that percentage in screen_buff
    // USAGE: LITERALLY
    static auto get_screen_percentage(double x_percentage, double y_percentage) -> decltype(screen_shape) {
        return std::make_tuple(
            (int)(std::get<0>(screen_shape) * x_percentage),
            (int)(std::get<1>(screen_shape) * y_percentage)
        );
    }

    // INPUT: percentage [0.0, 1.0] * 2 in match_buff
    // RETURN: point in match_buff (offset of screen_buff)
    // USAGE: LITERALLY
    // HINT: recommand to use get_match_shift
    static auto get_match_percentage(const std::tuple<double, double>& percentage) {
        return get_screen_percentage(std::get<0>(percentage), std::get<1>(percentage));
    }

    // INPUT: percentage [0.0, 1.0] * 2 in match_buff
    // RETURN: point in match_buff (offset of screen_buff)
    // USAGE: LITERALLY
    // HINT: recommand to use get_match_shift
    static auto get_match_percentage(double x_percentage, double y_percentage) {
        return std::make_tuple(
            (int)(std::get<0>(ref_shape) * x_percentage + std::get<0>(match_scale)),
            (int)(std::get<1>(ref_shape) * y_percentage + std::get<1>(match_scale))
        );
    }
    
    // INPUT: shift in match_buff, you can use shift = `shift::X_LEFT | shift::Y_DOWN`
    // RETURN: point in match_buff (offset of screen_buff)
    // USAGE: get the shifted point in match_buff
    //        e.g., `shift::X_EXTREME_LEFT | shift::Y_EXTREME_DOWN` will return the left-down point of match_buff
    //
    //                  x-extreme-left  x-left          x-center        x-right         x-extreme-right
    //  y-extreme-up    (1, 1)          (4, 1)          (8, 1)          (12, 1)         (15, 1)
    //  y-up            (1, 4)          (4, 4)          (8, 4)          (12, 4)         (15, 4)
    //  y-center        (1, 8)          (4, 8)          (8, 8)          (12, 8)         (15, 8)
    //  y-down          (1, 12)         (4, 12)         (8, 12)         (12, 12)        (15, 12)
    //  y-extreme-down  (1, 15)         (4, 15)         (8, 15)         (12, 15)        (15, 15)
    //
    static auto get_match_shift(int shift = shift::X_CENTER | shift::Y_CENTER) {
        auto grid = std::make_tuple(
            double(std::get<2>(match_scale) - std::get<0>(match_scale)) / 16,
            double(std::get<3>(match_scale) - std::get<1>(match_scale)) / 16
        );

        auto x_shift = std::get<0>(grid) * ((shift & 0xF0) >> 4);
        auto y_shift = std::get<1>(grid) * (shift & 0x0F);

        return std::make_tuple(
            int(std::get<0>(match_scale) + x_shift),
            int(std::get<1>(match_scale) + y_shift)
        );
    }

    // INPUT: threshold [0.0, 1.0], default is 0.8
    // RETURN: whether it is matched
    // USAGE: use ref_buff to match sub_screen_buff
    // HINT: resule scale position will save for `vision::get_match_xxx()`
    static bool match(double threshold = 0.8) {

        cv::Mat match_buff;
        match_buff.create(
            sub_screen_buff.cols - ref_buff.cols + 1,
            sub_screen_buff.rows - ref_buff.rows + 1, 
            CV_32FC1
        );

        // only use normed here, or threshold won't work
        const int method = cv::TM_CCOEFF_NORMED;
        double min_value, max_value, res_value;
        cv::Point res_location, min_location, max_location;

        cv::matchTemplate(sub_screen_buff, ref_buff, match_buff, method);
        cv::minMaxLoc(match_buff, &min_value, &max_value, &min_location, &max_location, cv::Mat());

        if (method == cv::TM_SQDIFF || method == cv::TM_SQDIFF_NORMED) {
            res_location = min_location;
            res_value = 1 - min_value;
        } else {
            res_location = max_location;
            res_value = max_value;
        }

        match_scale = std::make_tuple(
            res_location.x + std::get<0>(sub_screen_scale),
            res_location.y + std::get<1>(sub_screen_scale),
            res_location.x + std::get<0>(sub_screen_scale) + ref_buff.cols,
            res_location.y + std::get<1>(sub_screen_scale) + ref_buff.rows
        );

        // debug
        // cv::Mat rec_sub_screen_buff;
        // sub_screen_buff.copyTo(rec_sub_screen_buff);
        // cv::rectangle(
        //     rec_sub_screen_buff,
        //     cv::Point(std::get<0>(match_scale), std::get<1>(match_scale)),
        //     cv::Point(std::get<2>(match_scale), std::get<3>(match_scale)),
        //     cv::Scalar(0, 0, 255), 2, 8, 0
        // );
        // cv::namedWindow("test", cv::WINDOW_NORMAL);
        // cv::imshow("test", rec_sub_screen_buff);
        // cv::waitKey(0);
        // end debug

        bool matched = res_value > threshold;

        log_buff << "<vision> matching result: scale = " << match_scale
                 << ", value = " << res_value
                 << ", matched = " << std::boolalpha << matched << std::noboolalpha;
        logging::debug(log_buff);

        if (matched)
            return true;
        return false;
    }

};

cv::Mat vision::screen_buff, vision::sub_screen_buff, vision::ref_buff;
std::tuple<int, int> vision::screen_shape, vision::ref_shape;
std::tuple<int, int, int, int> vision::match_scale, vision::sub_screen_scale;

logging_buff vision::log_buff;
