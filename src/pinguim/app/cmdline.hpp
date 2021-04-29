#pragma once

#include <docopt.h>

#include <string_view>
#include <optional>
#include <charconv>
#include <regex>

#include "pinguim/cvt.hpp"

/// Summary.

namespace pinguim::app::cmdline
{
    static const constexpr char default_usage[] = R"(pbssl ver 0.0.

    Usage:
        pbssl [(--team=TEAM | -t=TEAM)] [--in-port=INPORT] [--in-address=INADDR] [--out-port=OUTPORT] [--out-address=OUTADDR] [--ref-address=ADDREF] [--ref-port=REFPORT] [--rep-port=REPPORT]

    Options:
        -h --help              Show this screen.
        --version              Show version.
        --team TEAM, -t TEAM   Set team color [default: blue].
        --in-port INPORT       Multicast group port    used to listen for fira_sim::sim_to_ref::Environment [default: 10002].
        --in-address INADDR    Multicast group address used to listen for fira_sim::sim_to_ref::Environment [default: 224.0.0.1].
        --out-port OUTPORT     Port    used to send fira_sim::sim_to_ref::Packet [default: 20011].
        --out-address OUTADDR  Address used to send fira_sim::sim_to_ref::Packet [default: 127.0.0.1].
        --ref-address ADDREF   Address used to communicate with the VSSRef [default: 224.0.0.1].
        --ref-port REFPORT     Port to receive information from the Referee [default: 10003].
        --rep-port REPPORT     Port to send replacement information to the Referee [default: 10004].
)";

    struct parsed_args
    {
        std::optional<std::string_view> in_address;
        std::optional<unsigned> in_port;

        std::optional<std::string_view> out_address;
        std::optional<unsigned> out_port;

        std::optional<std::string_view> referee_address;
        std::optional<unsigned> referee_port;

        std::optional<unsigned> rep_port;

        std::string_view team;
    };

    struct unwrapped_parsed_args
    {
        std::string in_address;
        std::uint16_t in_port;

        std::string out_address;
        std::uint16_t out_port;

        std::string referee_address;
        std::uint16_t referee_port;

        std::uint16_t rep_port;

        std::string team;

        inline unwrapped_parsed_args(const parsed_args& args)
            : in_address{ args.in_address.value() }
            , in_port{ cvt::toe << args.in_port.value() }
            , out_address{ args.out_address.value() }
            , out_port{ cvt::toe << args.out_port.value() }
            , referee_address{ args.referee_address.value() }
            , referee_port{ cvt::toe << args.referee_port.value() }
            , rep_port{ cvt::toe << args.rep_port.value() }
            , team{ args.team }
        {}
    };

    inline auto parse_argv(
        int argc,
        const char* argv[],
        const char* version_string = "pbssl ver 0.0",
        const char* usage = default_usage
    ) -> parsed_args;

    namespace parse
    {
        inline auto port(std::string_view input) -> std::optional<unsigned>;
        inline auto ipv4(std::string_view ipv4)  -> std::optional<std::string_view>;
    }
}

/// Implementations.
inline auto pinguim::app::cmdline::parse_argv(
    int argc,
    const char* argv[],
    const char* version_string,
    const char* usage
) -> parsed_args
{
    auto docopt_args = docopt::docopt(
        usage,
        {argv + 1, argv + argc},
        true,           // show help if requested
        version_string
    );

    return
    {
        .in_address = parse::ipv4(docopt_args["--in-address"].asString()),
        .in_port    = parse::port(docopt_args["--in-port"].asString()),

        .out_address = parse::ipv4(docopt_args["--out-address"].asString()),
        .out_port    = parse::port(docopt_args["--out-port"].asString()),

        .referee_address = parse::ipv4(docopt_args["--ref-address"].asString()),
        .referee_port    = parse::port(docopt_args["--ref-port"].asString()),

        .rep_port = parse::port(docopt_args["--rep-port"].asString()),

        .team = docopt_args["--team"].asString()
    };
}

inline auto pinguim::app::cmdline::parse::port(std::string_view input) -> std::optional<unsigned>
{
    unsigned result = 0;
    auto [ptr, error] = std::from_chars(input.data(), input.data() + input.size(), result);

    // error == std::errc() means all is ok.
    if(error != std::errc() || ptr != input.data() + input.size() || result < 1 || result > 65535)
        {return std::nullopt;}
    return result;
}

inline auto pinguim::app::cmdline::parse::ipv4(std::string_view ipv4) -> std::optional<std::string_view>
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
