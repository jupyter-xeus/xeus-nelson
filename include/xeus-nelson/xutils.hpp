/***************************************************************************
* Copyright (c) 2023, Hind Montassif
*
* Distributed under the terms of the BSD 3-Clause License.
*
* The full license is in the file LICENSE, distributed with this software.
****************************************************************************/

#ifndef X_UTILS_HPP
#define X_UTILS_HPP

#include <regex>

namespace xeus_nelson
{
    inline std::vector<std::string> split_line(const std::string& input, const std::string& delims, std::size_t cursor_pos)
    {
        // passing -1 as the submatch index parameter performs splitting
        std::vector<std::string> result;
        std::stringstream ss;

        ss << "[";
        for (auto c : delims)
        {
            ss << "\\" << c;
        }
        ss << "]";

        std::regex re(ss.str());

        std::copy(std::sregex_token_iterator(input.begin(), input.begin() + cursor_pos + 1, re, -1),
                  std::sregex_token_iterator(),
                  std::back_inserter(result));

        return result;
    }

    inline std::string trim(std::string const& str)
    {
        if (str.empty())
        {
            return str;
        }
        const std::string& chars_to_remove = " \t\n\r\f\v";
        std::size_t firstScan = str.find_first_not_of(chars_to_remove);
        std::size_t first = firstScan == std::string::npos ? str.length() : firstScan;
        std::size_t last = str.find_last_not_of(chars_to_remove);
        return str.substr(first, last - first + 1);
    }
}
#endif
