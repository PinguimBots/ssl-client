#pragma once

#include <string_view>
#include <optional>
#include <charconv>
#include <regex>

#include "pinguim/cvt.hpp"

namespace pinguim::app::cmdline::parse
{
    inline auto port(std::string_view input) -> std::optional<unsigned>
    {
        unsigned result = 0;
        auto [ptr, error] = std::from_chars(input.data(), input.data() + input.size(), result);

        // error == std::errc() means all is ok.
        if(error != std::errc() || ptr != input.data() + input.size() || result < 1 || result > 65535)
        { return std::nullopt; }
        return result;
    }

    inline auto ipv4(std::string_view ipv4) -> std::optional<std::string_view>
    {
        // Matches - roughly - a number in range [0, 255] (inclusive).
        #define PINGUIM_PARSE_IP_RANGE    \
            "("                        \
                "([0]+)"           "|" \
                "([0]*[0-9]*)"     "|" \
                "([0-9][0-9])"     "|" \
                "([0][0-9][0-9])"  "|" \
                "(1[0-9][0-9])"    "|" \
                "([2][0-4][0-9])"  "|" \
                "(25[0-5])"            \
            ")"
        #define PINGUIM_PARSE_DOT "\\."

        // read: [0-255].[0-255].[0-255].[0-255]
        std::regex regexpr(
                        PINGUIM_PARSE_IP_RANGE
            PINGUIM_PARSE_DOT PINGUIM_PARSE_IP_RANGE
            PINGUIM_PARSE_DOT PINGUIM_PARSE_IP_RANGE
            PINGUIM_PARSE_DOT PINGUIM_PARSE_IP_RANGE
        );
        #undef PINGUIM_PARSE_DOT
        #undef PINGUIM_PARSE_IP_RANGE

        // regex_match needs the *exact* sequence to match.
        if(std::regex_match(ipv4.data(), regexpr)) {
            return ipv4;
        }

        return {};
    }
}