#include <trak/input.h>
#include <print>
#include <vector>
#include <ranges>
#include <algorithm>

int main()
{
    auto input = trak::read_file("day01/input.input");

    std::println("--- Day 1: Trebuchet?! ---");
    std::println("Part 1: {}",
        std::ranges::fold_left(
            input
            | std::views::split('\n')
            | std::views::transform([](auto&& line) -> int {
                auto first = std::ranges::find_if(line, isdigit);
                auto [last, _] = std::ranges::find_last_if(line, isdigit);
                return 10 * (*first - '0') + (*last - '0');
            }), 0, std::plus<>()));

    std::vector<std::pair<std::string_view, int>> number_map {
        {"0",0}, {"1",1}, {"2",2}, {"3",3}, {"4",4},
        {"5",5}, {"6",6}, {"7",7}, {"8",8}, {"9",9},
        {"one",1}, {"two",2}, {"three",3}, {"four",4}, {"five",5},
        {"six",6}, {"seven",7}, {"eight",8}, {"nine",9}
    };

    std::println("Part 2: {}",
        std::ranges::fold_left(
            input
            | std::views::split('\n')
            | std::views::transform([](auto&& line) { return std::string_view(line); })
            | std::views::transform(
                [&](auto&& line)->int {
                    auto [_0, first] = *std::ranges::min_element(number_map, [line](auto&& a, auto&& b) {
                        return line.find(a.first) < line.find(b.first);
                    });
                    auto [_1, last] = *std::ranges::max_element(number_map, [line](auto&& a, auto&& b) {
                        return static_cast<int64_t>(line.rfind(a.first)) < static_cast<int64_t>(line.rfind(b.first));
                    });
                    return 10 * first + last;
                }
            ), 0, std::plus<>())
    );
}