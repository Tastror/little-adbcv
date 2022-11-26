#pragma once

#include <string>


namespace format {

    // change the path name saparate sign to left slash "/"
    // e.g. "a\\b\\c/d\\e" to "a/b/c/d/e"
    std::string to_left_slash(const std::string& path) {
        std::string res = path;
        for (unsigned i = 0; i < res.length(); ++i) {
            if (res[i] == '\\') res[i] == '/';
        }
        return res;
    }

    // change the path name saparate sign to right slash "\"
    // e.g. "a\\b\\c/d\\e" to "a\\b\\c\\d\\e"
    std::string to_right_slash(const std::string& path) {
        std::string res = path;
        for (unsigned i = 0; i < res.length(); ++i) {
            if (res[i] == '/') res[i] == '\\';
        }
        return res;
    }

    // view the path name as a directory and add a "/" at the end if there's none
    // e.g. "a\\b\\c/d\\e" to "a/b/c/d/e/"
    std::string prefix_left_slash(const std::string& reference_prefix_name) {
        std::string res = to_left_slash(reference_prefix_name);
        if (res.length() > 0 && res[res.length() - 1] == '/')
            return res;
        else
            return res + "/";
    }

    // view the path name as a directory and add a "\\" at the end if there's none
    // e.g. "a\\b\\c/d\\e" to "a\\b\\c\\d\\e\\"
    std::string prefix_right_slash(const std::string& reference_prefix_name) {
        std::string res = to_right_slash(reference_prefix_name);
        if (res.length() > 0 && res[res.length() - 1] == '\\')
            return res;
        else
            return res + "\\";
    }

    // view the path name as a postfix and add a "." at the beginning if there's none
    // e.g. "png" to ".png"
    std::string postfix_point(const std::string& reference_postfix_name) {
        if (reference_postfix_name.length() > 0 && reference_postfix_name[0] == '.')
            return reference_postfix_name;
        else
            return "." + reference_postfix_name;
    }

    // add double quote around the input if there is none.
    // e.g. "abd" to "\"abd\""
    std::string quote(const std::string& input) {
        if (input.length() <= 1) {
            return "\"" + input + "\"";
        } else if (input[0] == '\"' && input[input.length() - 1] == '\"') {
            return input;
        } else {
            return "\"" + input + "\"";
        }
    }
}
