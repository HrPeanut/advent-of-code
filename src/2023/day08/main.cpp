#include <trak/input.h>

#include <print>
#include <ranges>
#include <algorithm>
#include <unordered_map>
#include <numeric>

using namespace std::literals;

using digraph = std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>>;

size_t traverse_part_1(const digraph& graph, std::string_view steps) {
    auto src = "AAA"sv;
    size_t count = 0;
    for (std::string_view view : std::views::repeat(steps)) {
        for (char c : view) {
            if (src == "ZZZ"sv)
                return count;
            auto& pair = graph.at(src);
            src = c == 'L' ? pair.first : pair.second;
            ++count;
        }
    }
}

size_t traverse_part_2(const digraph& graph, std::string_view src, std::string_view steps) {
    size_t count = 0;
    for (std::string_view view : std::views::repeat(steps)) {
        for (char c : view) {
            if (src.ends_with('Z'))
                return count;
            auto& pair = graph.at(src);
            src = c == 'L' ? pair.first : pair.second;
            ++count;
        }
    }
}

int main()
{
    auto input = trak::read_file("day08/input.input");
    auto lines = std::views::split(input, '\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& rng) { return std::string_view(rng); });

    auto steps = std::views::take(lines, 1).front();

    digraph graph;
    for (auto view : std::views::drop(lines, 1) | std::ranges::to<std::vector>())
        graph[view.substr(0, 3)] = std::make_pair(view.substr(7, 3), view.substr(12, 3));

    std::println("--- Day 8: Haunted Wasteland ---");
    std::println("Part 1: {}", traverse_part_1(graph, steps));
    std::println("Part 2: {}",
        std::ranges::fold_left(
            graph
            | std::views::transform([](auto&& kv) { return kv.first; })
            | std::views::filter([](auto node) { return node.ends_with('A'); })
            | std::views::transform([&](auto node) { return traverse_part_2(graph, node, steps); }),
            1,
            [](auto a, auto b) { return std::lcm(a, b);}));
}