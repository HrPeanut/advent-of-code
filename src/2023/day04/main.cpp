#include <trak/input.h>
#include <print>
#include <string>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <unordered_set>

int main()
{
    auto input = trak::read_file("day04/input.input");

    std::println("--- Day 4: Scratchcards ---");
    std::println("Part 1: {}",
        std::ranges::fold_left(
            input
            | std::views::split('\n')
            | std::views::transform([](auto&& line) { return std::string_view(line); })
            | std::views::transform([](std::string_view&& line) {
                auto colon = line.find(':');
                auto vertical = line.find('|');
                auto numbers = line.substr(colon + 1, vertical - colon - 1)
                    | std::views::split(' ')
                    | std::views::transform([](auto&& num) { return std::string_view(num); })
                    | std::views::filter([](std::string_view num) { return num.find_first_not_of(' ') != std::string_view::npos; })
                    | std::views::transform([](std::string_view num) { return std::stoi(num.data()); })
                    | std::ranges::to<std::unordered_set>();

                auto wins = std::ranges::count_if(
                    line.substr(vertical + 1)
                    | std::views::split(' ')
                    | std::views::transform([](auto&& num) { return std::string_view(num); })
                    | std::views::filter([](std::string_view num) { return num.find_first_not_of(' ') != std::string_view::npos; })
                    | std::views::transform([](std::string_view num) { return std::stoi(num.data()); }),
                    [numbers](int val) { return numbers.contains(val); });

                return wins == 0 ? 0 : 1 << (wins - 1);
            }
            ), 0, std::plus<>()));
}