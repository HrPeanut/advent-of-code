#include <trak/input.h>
#include <print>
#include <ranges>
#include <algorithm>

int main()
{
    auto input = trak::read_file("day06/input.input");
    auto lines =
        std::views::split(input, '\n')
        | std::views::transform([](auto&& d) { return std::string_view(d); });

    auto parse_numbers = [](std::string_view view) {
        return std::views::drop(view, 10)
        | std::views::split(' ')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& rng) { return std::stoi(std::data(rng)); })
        | std::ranges::to<std::vector>();
    };

    std::vector<int> times = parse_numbers(lines.front());
    std::vector<int> distances = parse_numbers(std::views::drop(lines, 1).front());

    std::println("--- Day 6: Wait For It ---");

    auto calculate_wins = [](uint64_t time, uint64_t distance) {
        int wins = 0;
        for (int i = 0; i <= time; ++i) {
            if ((time - i) * i > distance)
                ++wins;
        }
        return wins;
    };

    std::println("Part 1: {}",std::ranges::fold_left(std::views::zip_transform(calculate_wins, times, distances), 1, std::multiplies()));

    auto parse_number_ignore_space = [](std::string_view view) {
        auto str = std::views::drop(view, 10)
                   | std::views::filter([](char c) { return c != ' '; })
                   | std::ranges::to<std::string>();
        return std::stoull(str);
    };

    auto race_time = parse_number_ignore_space(lines.front());
    auto race_dist = parse_number_ignore_space(std::views::drop(lines, 1).front());
    std::println("Part 2: {}",calculate_wins(race_time, race_dist));
}