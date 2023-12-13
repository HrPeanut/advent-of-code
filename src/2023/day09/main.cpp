#include <trak/input.h>
#include <print>
#include <ranges>
#include <algorithm>

int64_t predict_forward(const std::vector<int64_t>& numbers) {
    if (std::ranges::all_of(numbers, [](auto x) { return x == 0; }))
        return 0;

    auto diff = numbers
        | std::views::slide(2)
        | std::views::transform([](auto&& x) { return x.back() - x.front(); })
        | std::ranges::to<std::vector>();

    return numbers.back() + predict_forward(diff);
}

int64_t predict_backward(const std::vector<int64_t>& numbers) {
    if (std::ranges::all_of(numbers, [](auto x) { return x == 0; }))
        return 0;

    auto diff = numbers
        | std::views::reverse
        | std::views::slide(2)
        | std::views::transform([](auto&& x) {
            return x.front() - x.back();
        })
        | std::views::reverse
        | std::ranges::to<std::vector>();

    return numbers.front() - predict_backward(diff);
}

int main() {
    auto input = trak::read_file("day09/input.input");
    auto lines = std::views::split(input, '\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& rng) { return std::string_view(rng); });

    std::println("--- Day 9: Mirage Maintenance ---");
    std::println("Part 1: {}",
        std::ranges::fold_left(
            std::views::transform(lines, [](auto line) {
                return predict_forward(line
                    | std::views::split(' ')
                    | std::views::filter([](auto&& rng) { return !std::empty(rng); })
                    | std::views::transform([](auto&& rng) { return std::stoll(std::data(rng)); })
                    | std::ranges::to<std::vector>());
            }),
            0,
            std::plus<>()));
    std::println("Part 2: {}",
        std::ranges::fold_left(
            std::views::transform(lines, [](auto line) {
                auto pred =
                 predict_backward(line
                    | std::views::split(' ')
                    | std::views::filter([](auto&& rng) { return !std::empty(rng); })
                    | std::views::transform([](auto&& rng) { return std::stoll(std::data(rng)); })
                    | std::ranges::to<std::vector>());
                return pred;
            }),
            0,
            std::plus<>()));
}