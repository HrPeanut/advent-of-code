#include <trak/input.h>
#include <print>
#include <string>
#include <ranges>
#include <algorithm>

struct seed_range { uint64_t src, length; };
struct almanac_range { uint64_t src, dst, length; };
using almanac_map = std::vector<almanac_range>;

auto parse_almanac_map(std::string_view view) {
    almanac_map parsed_ranges;

    for (auto&& rng : view | std::views::split('\n') | std::views::drop(1)) {
        if (rng.empty()) break;

        auto numbers = std::string_view(rng)
            | std::views::split(' ')
            | std::views::transform([](auto&& rng) -> uint64_t { return std::stoull(std::data(rng)); })
            | std::ranges::to<std::vector>();

        parsed_ranges.push_back({
            .src = numbers[1],
            .dst = numbers[0],
            .length = numbers[2]
        });
    }

    std::ranges::sort(parsed_ranges, [](auto& a, auto& b) { return a.src < b.src; });
    return parsed_ranges;
}

auto fill_almanac_ranges(const almanac_map& parsed_ranges) {
    almanac_map ranges;
    uint64_t src = 0;
    for (const auto& range : parsed_ranges) {
        if (src != range.src)
            ranges.push_back({ .src = 0, .dst = 0, .length = range.src });
        ranges.push_back(range);
        src = range.src + range.length;
    }
    ranges.push_back({ .src = src, .dst = src, .length = UINT64_MAX - src });
    return ranges;
}

auto map_seed(uint64_t src, const almanac_map& map) {
    for (auto& range : map)
    {
        if (range.src <= src && src < range.src + range.length)
            return src + (range.dst - range.src);
    }
    return src;
}

auto map_range(const std::vector<seed_range>& src_ranges, const almanac_map& map) {
    std::vector<seed_range> dst_ranges;

    for (const seed_range& s : src_ranges) {
        // Find first range, that s intersects
        auto it = std::ranges::find_if(map, [s](const almanac_range& m) -> bool {
            return m.src <= s.src && s.src < m.src + m.length;
        });

        auto src = s.src;
        auto len = s.length;
        while (src < s.src + s.length) {
            auto dst_src = src + (it->dst - it->src);
            auto dst_len = std::min(len, it->src + it->length - src);

            dst_ranges.push_back({ .src = dst_src, .length = dst_len });

            src += dst_len;
            len -= dst_len;
            ++it;
        }
    }

    return dst_ranges;
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
        | std::views::transform(fill_almanac_ranges)
        | std::ranges::to<std::vector>();

    std::println("--- Day 5: If You Give A Seed A Fertilizer ---");
    std::println("Part 1: {}",
        std::ranges::fold_left(
            seeds
            | std::views::transform([&](uint64_t seed) -> uint64_t {
                return std::ranges::fold_left(almanac, seed, map_seed);
            }), UINT64_MAX, [](uint64_t a, uint64_t b) { return std::min(a, b); }));

    auto seed_ranges = seeds
        | std::views::chunk(2)
        | std::views::transform([&](auto rng) { return seed_range(rng.front(), rng.back()); })
        | std::ranges::to<std::vector>();

    std::println("Part 2: {}",
        std::ranges::fold_left(
            std::ranges::fold_left(almanac, seed_ranges, map_range),
            UINT64_MAX,
            [](uint64_t a, seed_range& b) { return std::min(a, b.src); }));

    return 0;
}