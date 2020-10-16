#pragma once

#include <string_view>
#include <optional>
#include <charconv>
#include <regex>

namespace pbts::parse {
    auto port(std::string_view input) -> std::optional<unsigned>
    {
        unsigned result = 0;
        auto [ptr, error] = std::from_chars(input.begin(), input.end(), result);

        // error == std::errc() means all is ok. 
        if(error != std::errc() || ptr != input.end() || result < 1 || result > 65535)
            {return std::nullopt;}
        return result;
    }

    auto ipv4(std::string_view ipv4) -> bool {
        #define PBTS_PARSE_IP_RANGE    \
            "("                        \
                "([0]+)"           "|" \
                "([0]*[0-9]*)"     "|" \
                "([0-9][0-9])"     "|" \
                "([0][0-9][0-9])"  "|" \
                "(1[0-9][0-9])"    "|" \
                "([2][0-4][0-9])"  "|" \
                "(25[0-5])"            \
            ")"

        std::regex regexpr(
            PBTS_PARSE_IP_RANGE
            "\\." PBTS_PARSE_IP_RANGE
            "\\." PBTS_PARSE_IP_RANGE
            "\\." PBTS_PARSE_IP_RANGE
        );

        return std::regex_match(ipv4.data(), regexpr); // regex_match needs to match the exact sequence.

        #undef PBTS_PARSE_IP_RANGE
    }

}
