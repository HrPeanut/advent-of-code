#include <trak/input.h>
#include <trak/graph.h>
#include <print>
#include <ranges>
#include <queue>

struct position { int row, col; };

int main() {
    auto input = trak::read_file("day10/input.input");
    auto lines = std::views::split(input, '\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); });
    auto pipes = input
        | std::views::filter([](char c) { return c != '\n'; })
        | std::ranges::to<std::string>();

    auto height = std::ranges::distance(lines);
    auto width = lines.front().size();

    auto index = [&](auto pos) { return pos.row * width + pos.col; };
    auto from_index = [&](auto pos) { return position{
        .row = static_cast<int>(pos / width),
        .col = static_cast<int>(pos - static_cast<int>(pos / width) * width)
    };};
    auto north = [&](auto pos) { return position(pos.row - 1, pos.col); };
    auto south = [&](auto pos) { return position(pos.row + 1, pos.col); };
    auto west = [&](auto pos) { return position(pos.row, pos.col - 1 ); };
    auto east = [&](auto pos) { return position(pos.row, pos.col + 1 ); };

    auto get_pipe = [&](auto pos) {
        return 0 <= pos.row && pos.row < height && 0 <= pos.col && pos.col < width ? pipes[index(pos)] : '.';
    };

    auto has_north = [&](char pipe) { return pipe == '|' || pipe == 'L' || pipe == 'J' | pipe == 'S'; };
    auto has_south = [&](char pipe) { return pipe == '|' || pipe == '7' || pipe == 'F' | pipe == 'S'; };
    auto has_west = [&](char pipe) { return pipe == '-' || pipe == 'J' || pipe == '7' | pipe == 'S'; };
    auto has_east = [&](char pipe) { return pipe == '-' || pipe == 'L' || pipe == 'F' | pipe == 'S'; };

    trak::graph<int> graph(pipes.size(), -1);
    for (int i = 0; i < pipes.size(); ++i) {
        auto pipe = pipes[i];
        // North
        if (has_north(pipe)) {
            auto pos = north(from_index(i));
            if (has_south(get_pipe(pos))) graph.add_edge(i, index(pos));
        }
        // South
        if (has_south(pipe)) {
            auto pos = south(from_index(i));
            if (has_north(get_pipe(pos))) graph.add_edge(i, index(pos));
        }
        // West
        if (has_west(pipe)) {
            auto pos = west(from_index(i));
            if (has_east(get_pipe(pos))) graph.add_edge(i, index(pos));
        }
        // East
        if (has_east(pipe)) {
            auto pos = east(from_index(i));
            if (has_west(get_pipe(pos))) graph.add_edge(i, index(pos));
        }
    }

    auto start = pipes.find('S');
    graph[start].value() = 0;

    std::queue<size_t> queue;
    queue.push(start);

    while (!queue.empty()) {
        auto& node = graph[queue.front()];
        queue.pop();

        for(const auto& edge : node.edges()) {
            auto& to = graph[edge.to()];
            if (to.value() != -1) continue;
            queue.push(edge.to());
            to.value() = node.value() + 1;
        }
    }

    std::vector<int> dsd;
    std::println("--- Day 10: Pipe Maze ---");
    std::println("Part 1: {}",
        std::ranges::fold_left(graph.nodes(), 0, [](auto a, auto& b) -> int {
            return std::max(a, b.value());
        }));
}