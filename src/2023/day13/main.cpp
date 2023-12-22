#include <trak/input.h>
#include <print>
#include <ranges>
#include <unordered_map>
#include <algorithm>
#include <numeric>

template<typename T>
auto find_reflection(const std::vector<T>& grid, int smudges) {
    for (int i = 1; i < grid.size(); ++i) {
        auto above = grid | std::views::take(i) | std::views::reverse | std::ranges::to<std::vector>();
        auto below = grid | std::views::drop(i) | std::ranges::to<std::vector>();

        auto d0 =
            std::views::zip(
                grid | std::views::take(i) | std::views::reverse,
                grid | std::views::drop(i))
            | std::views::transform([](auto&& rng) {
                auto fisk = std::views::zip(rng.first, rng.second)
                        | std::views::transform([](auto&& rng) { return rng.first == rng.second ? 0 : 1; });
                return std::accumulate(std::begin(fisk), std::end(fisk), 0);
            });
        auto sum = std::accumulate(d0.begin(), d0.end(), 0);

        if (sum == smudges)
            return i;
    }

    return 0;
}

auto find_symmetry(std::string_view view, int smudge) {
    auto rows = view | std::views::split('\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& rng) { return std::string_view(rng); })
        | std::ranges::to<std::vector>();

    if (auto result = find_reflection(rows, smudge); result != 0) {
        return result * 100;
    }

    std::vector<std::string> transpose(rows.front().size(), std::string(rows.size(), '-'));
    for (std::vector<int>::size_type i = 0; i < rows[0].size(); i++)
        for (std::vector<int>::size_type j = 0; j < rows.size(); j++) {
            transpose[i][j] = rows[j][i];
        }

    return find_reflection(transpose, smudge);
}

int main() {
    using namespace std::literals;

    auto input = trak::read_file("day13/input.input");
    auto patterns = std::views::split(input, "\n\n"sv)
        | std::views::transform([](auto&& d) { return std::string_view(d); });

    std::println("--- Day 13: Point of Incidence ---");

    auto part1 = patterns | std::views::transform([](auto&& pattern) { return find_symmetry(pattern, 0); });
    std::println("Part 1: {}", std::accumulate(std::begin(part1), std::end(part1), 0));

    auto part2 = patterns | std::views::transform([](auto&& pattern) { return find_symmetry(pattern, 1); });
    std::println("Part 2: {}", std::accumulate(std::begin(part2), std::end(part2), 0));
}