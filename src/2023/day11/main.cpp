#include <trak/input.h>
#include <print>
#include <ranges>
#include <algorithm>

int main() {
    auto input = trak::read_file("day11/input.input");
    auto lines = std::views::split(input, '\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& rng) { return std::string_view(rng); });

    auto width = static_cast<int64_t>(lines.front().size());

    auto to_index = [](auto&& t) -> int64_t { return static_cast<int64_t>(std::get<0>(t)); };
    auto all_dots = [](auto&& t) -> bool { return std::ranges::all_of(std::get<1>(t), [](char c) { return c == '.'; }); };

    auto empty_rows = lines
        | std::views::enumerate
        | std::views::filter(all_dots)
        | std::views::transform(to_index)
        | std::ranges::to<std::vector>();

    auto column_view = [&](int offset) { return input | std::views::drop(offset) | std::views::stride(width + 1); };
    auto empty_cols = std::views::iota(0, width)
        | std::views::transform(column_view)
        | std::views::enumerate
        | std::views::filter(all_dots)
        | std::views::transform(to_index)
        | std::ranges::to<std::vector>();

    struct galaxy { int64_t row, col; };
    auto create_galaxies = [&](int64_t age = 2) {
        return std::views::enumerate(lines)
            | std::views::transform([&](auto&& rng_row) {
            auto&& [row, line] = rng_row;
            return std::views::enumerate(line)
                | std::views::filter([](auto&& rng_col) { return std::get<1>(rng_col) == '#'; })
                | std::views::transform([age, row, &empty_rows, &empty_cols](auto&& rng_col) {
                auto&& [col, _] = rng_col;
                return galaxy{
                    .row = static_cast<int64_t>(row) + (age - 1) * std::ranges::count_if(empty_rows, [&](auto r) { return r < row; }),
                    .col = static_cast<int64_t>(col) + (age - 1) * std::ranges::count_if(empty_cols, [&](auto c) { return c < col; })
                }; });
        })
            | std::views::join
            | std::ranges::to<std::vector>();
    };

    auto manhattan = [](galaxy& a, galaxy& b) -> int64_t { return std::abs(a.row - b.row) + std::abs(a.col - b.col); };

    std::println("--- Day 11: Cosmic Expansion ---");
    auto galaxies = create_galaxies();
    int64_t sum = 0;
    for (int i = 0; i < galaxies.size(); ++i) {
        for (int j = i + 1; j < galaxies.size(); ++j) {
            sum += manhattan(galaxies[i], galaxies[j]);
        }
    }
    std::println("Part 1: {}", sum);

    galaxies = create_galaxies(1000000);
    sum = 0;
    for (int i = 0; i < galaxies.size(); ++i) {
        for (int j = i + 1; j < galaxies.size(); ++j) {
            sum += manhattan(galaxies[i], galaxies[j]);
        }
    }
    std::println("Part 2: {}", sum);
}