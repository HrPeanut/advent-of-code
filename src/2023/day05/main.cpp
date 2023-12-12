#include <trak/input.h>
#include <print>
#include <string>
#include <ranges>
#include <algorithm>
#include <iostream>

struct mapped_range { uint64_t src, dst, length; };
using almanac_map = std::vector<mapped_range>;

uint64_t find_dst(std::string_view view, uint64_t src) {
    for (auto&& rng: view | std::views::split('\n') | std::views::drop(1)) {
        std::string_view row(rng);
        if (rng.empty()) continue;

        auto numbers =
            row
            | std::views::split(' ')
            | std::views::transform([](auto&& rng) -> uint64_t { return std::stoull(std::data(rng)); })
            | std::ranges::to<std::vector>();

        auto i_dst = numbers[0];
        auto i_src = numbers[1];
        auto i_len = numbers[2];

        if (i_src <= src && src < i_src + i_len) {
            return i_dst + src - i_src;
        }
    }

    return src;
}

auto parse_almanac_map(std::string_view view) {
    almanac_map ranges;

    for (auto&& rng : view | std::views::split('\n') | std::views::drop(1)) {
        if (rng.empty()) break;

        auto numbers = std::string_view(rng)
            | std::views::split(' ')
            | std::views::transform([](auto&& rng) -> uint64_t { return std::stoull(std::data(rng)); })
            | std::ranges::to<std::vector>();

        ranges.push_back({
            .src = numbers[1],
            .dst = numbers[0],
            .length = numbers[2]
        });
    }

    return ranges;
}

auto map_src_to_dst(uint64_t src, const almanac_map& map) {
    for (auto& range : map)
    {
        if (range.src <= src && src < range.src + range.length)
            return range.dst + src - range.src;
    }
    return src;
}

int main()
{
    using std::operator""sv;
    auto input = trak::read_file("day05/input.input");
    auto split =
        std::views::split(input, "\n\n"sv)
        | std::views::transform([](auto&& d) { return std::string_view(d); });

    auto seeds = std::views::drop(split.front(), 7)
        | std::views::split(' ')
        | std::views::transform([](auto&& rng) -> uint64_t { return std::stoull(std::data(rng)); })
        | std::ranges::to<std::vector>();

    auto almanac = std::views::drop(split, 1)
        | std::views::transform(parse_almanac_map)
        | std::ranges::to<std::vector>();

    std::println("--- Day 5: If You Give A Seed A Fertilizer ---");
    std::println("Part 1: {}",
        std::ranges::fold_left(
            seeds
            | std::views::transform([&](uint64_t seed) -> uint64_t {
                return std::ranges::fold_left(almanac, seed, map_src_to_dst);
            }), UINT64_MAX, [](uint64_t acc, uint64_t val) -> uint64_t { return std::min(acc, val); }));

//    std::println("Part 2: {}",
//        std::ranges::fold_left(
//            seeds
//            | std::views::chunk(2)
//            | std::views::transform([&](auto rng) { return std::views::iota(rng.front(), rng.front() + rng.back()); })
//            | std::views::join
//              | std::views::transform([&](uint64_t seed) -> uint64_t {
//                return std::ranges::fold_left(
//                    split | std::views::drop(1) // Skip seeds line
//                    , seed, [](uint64_t src, auto&& rng) -> uint64_t { return find_dst(rng, src); });
//            })
//            , UINT64_MAX, [](uint64_t acc, uint64_t val) -> uint64_t { return std::min(acc, val); }
//            ));

    return 0;
}