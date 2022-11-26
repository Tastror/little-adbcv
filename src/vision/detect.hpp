#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <tuple>
#include <set>

#include "common/stream.hpp"
#include "common/format.hpp"
#include "common/log.hpp"


class detect {

private:

    static cv::Mat screen_buff, sub_screen_buff;
    // (x, y) or (cols, rows)
    static std::tuple<int, int> screen_shape;
    // (left-up x, left-up y, right-down x, right-down y)
    static std::tuple<int, int, int, int> sub_screen_scale;

    static std::vector<cv::Mat> collections;
    // collections' names
    static std::vector<std::string> collections_id;
    // (x, y) or (cols, rows)
    static std::tuple<int, int> collections_shape;

    static logging_buff log_buff;

public:
    // INPUT: pictures' directory full path ("\\" or "/" or null at the end are all OK),
    //        vector of pictures' names or sub_path (spaces, dots, slashes allowed, but no "\\" or "/" at the beginning or end),
    //        pictures' same postfix ("." or null at the beginning are both OK)
    // RETURN: collections vector of cv::Mat
    // USAGE: LITERALLY
    // HINT: in order to make the detect faster,
    //       this will use the first picture's shape to cut all other pictures from left-up,
    //       so make sure all pictures are the same size or similar size
    //       moreover, the collection_id will assigned by collection_names
    static const std::vector<cv::Mat>& read_collections(
        const std::string& dir_path,
        const std::vector<std::string>& collection_names,
        const std::string& postfix
    ) {
        const std::string slashed_dir_path = format::prefix_right_slash(postfix);
        const std::string pointed_postfix = format::postfix_point(postfix);

        // empty collections
        collections.clear();
        collections_id.clear();

        // get first shape
        if (collection_names.size() < 1) return collections;
        const std::string first_path = slashed_dir_path + format::to_right_slash(collection_names[0]) + pointed_postfix;
        cv::Mat temp_buff = cv::imread(first_path);
        collections_shape = std::make_tuple(temp_buff.cols, temp_buff.rows);
        const int cv_type = temp_buff.type();

        log_buff << "<detect> reading first collection " << format::quote(first_path) << " of " << collections_shape;
        logging::debug(log_buff);

        // get all
        for (const auto& name : collection_names) {
            cv::Mat new_buff;
            new_buff.create(
                std::get<0>(collections_shape),
                std::get<1>(collections_shape),
                cv_type
            );
            const std::string now_path = slashed_dir_path + name + pointed_postfix;
            temp_buff = cv::imread(now_path);
            temp_buff.copyTo(new_buff);
            collections.push_back(new_buff);
            collections_id.push_back(name);

            log_buff << "<detect> reading collections " << format::quote(now_path) << " of (maybe forced) " << collections_shape;
            logging::debug(log_buff);

            // debug
            // cv::imshow(name, new_buff);
            // cv::waitKey(0);
            // end debug
        }
        return collections;
    }

    // INPUT: full path of the screen picture
    // RETURN: screen_buff
    // USAGE: LITERALLY
    // HINT: once read, the sub_screen_buff will be the same as screen_buff
    static const cv::Mat& read_screen(std::string path) {
        screen_buff = cv::imread(path);
        screen_shape = std::make_tuple(screen_buff.cols, screen_buff.rows);

        log_buff << "<detect> reading screen " << format::quote(path) << " of " << screen_shape;
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

        sub_screen_scale = sub_scale;;
        int x, y, z, w;
        std::tie(x, y, z, w) = sub_screen_scale;
        cv::Rect sub_rect(x, y, z - x, w - y);
        sub_screen_buff = screen_buff(sub_rect);

        log_buff << "<detect> sub screen scale = " << sub_screen_scale;
        logging::debug(log_buff);

        // debug
        // cv::imshow("sub_screen", sub_screen_buff);
        // cv::waitKey(0);
        // end debug

        return sub_screen_buff;
    }

    // INPUT: threshold [0.0, 1.0], default is 0.75
    // RETURN: which collections are matched
    // USAGE: use pictures in collections to match sub_screen_buff
    static std::vector<std::string> match_all(double threshold = 0.75) {

        std::vector<std::string> result_set;

        cv::Mat match_buff;
        match_buff.create(
            sub_screen_buff.cols - std::get<0>(collections_shape) + 1,
            sub_screen_buff.rows - std::get<1>(collections_shape) + 1, 
            CV_32FC1
        );

        // only use normed here, or threshold won't work
        const int method = cv::TM_CCOEFF_NORMED;
        double min_value, max_value, res_value;
        cv::Point res_location, min_location, max_location;

        // detect all
        for (int i = 0; i < collections.size(); ++i) {

            const auto& buff = collections[i];
            const auto& name = collections_id[i];

            cv::matchTemplate(sub_screen_buff, buff, match_buff, method);
            cv::minMaxLoc(match_buff, &min_value, &max_value, &min_location, &max_location, cv::Mat());
            if (method == cv::TM_SQDIFF || method == cv::TM_SQDIFF_NORMED) {
                res_location = min_location;
                res_value = 1 - min_value;
            } else {
                res_location = max_location;
                res_value = max_value;
            }

            // debug
            // cv::Mat rec_screen_buff;
            // auto temp_scale = std::make_tuple(
            //     res_location.x, res_location.y,
            //     res_location.x + buff.cols, res_location.y + buff.rows
            // );
            // sub_screen_buff.copyTo(rec_screen_buff);
            // cv::rectangle(
            //     rec_screen_buff,
            //     cv::Point(std::get<0>(temp_scale), std::get<1>(temp_scale)),
            //     cv::Point(std::get<2>(temp_scale), std::get<3>(temp_scale)),
            //     cv::Scalar(0, 0, 255), 2, 8, 0
            // );
            // cv::imshow(name, rec_screen_buff);
            // cv::waitKey(0);
            // end debug

            bool matched = res_value > threshold;

            log_buff << "<detect> matching result: name = " << name
                     << ", value = " << res_value
                     << ", matched = " << std::boolalpha << matched << std::noboolalpha;
            logging::debug(log_buff);

            if (matched) result_set.push_back(name);

        }
        return result_set;
    }

};

cv::Mat detect::screen_buff;
cv::Mat detect::sub_screen_buff;
std::tuple<int, int> detect::screen_shape;
std::tuple<int, int, int, int> detect::sub_screen_scale;

std::vector<cv::Mat> detect::collections;
std::vector<std::string> detect::collections_id;
std::tuple<int, int> detect::collections_shape;

logging_buff detect::log_buff;
