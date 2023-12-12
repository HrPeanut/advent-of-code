#include <trak/input.h>
#include <print>
#include <ranges>
#include <algorithm>
#include <unordered_set>

int parse_wins(std::string_view line) {
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

    return static_cast<int>(wins);
}

int main()
{
    auto input = trak::read_file("day04/input.input");
    auto wins =
        input
        | std::views::split('\n')
        | std::views::transform([](auto&& line) { return std::string_view(line); })
        | std::views::filter([](std::string_view num) { return num.find_first_not_of(' ') != std::string_view::npos; })
        | std::views::transform(parse_wins)
        | std::ranges::to<std::vector>();

    std::println("--- Day 4: Scratchcards ---");
    std::println("Part 1: {}",
        std::ranges::fold_left(
            wins
            | std::views::transform([](int wins) { return wins == 0 ? 0 : 1 << (wins - 1); }
            ), 0, std::plus()));

    std::vector<int> cards(wins.size(), 1);
    std::println("Part 2: {}",
        std::ranges::fold_left(
            std::views::iota(0, static_cast<int>(wins.size()))
            | std::views::transform([&](int i) -> int {
                auto num_cards = cards[i];
                auto num_wins = wins[i];
                for (int j = 0; j < num_wins; ++j)
                    cards[i + j + 1] += num_cards;
                return i;
            })
            | std::views::transform([&](int i) -> int {
                return cards[i];
            }) , 0, std::plus()));

    return 0;
}