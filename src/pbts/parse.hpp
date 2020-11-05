#pragma once

#include <string_view>
#include <optional>
#include <charconv>
#include <regex>

namespace pbts::parse {
    auto port(std::string_view input) -> std::optional<unsigned>;
    auto ipv4(std::string_view ipv4) -> std::optional<std::string_view>;
}
