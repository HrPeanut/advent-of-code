#include <trak/input.h>
#include <trak/graph.h>
#include <print>
#include <ranges>
#include <queue>
#include <iostream>
#include <locale>
#include <string>
#include <codecvt>

struct position { int row, col; };

int main() {
    // ================================= Part 1 =================================

    auto input = trak::read_file("day10/input.input");
    auto lines = std::views::split(input, '\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); });
    auto pipes = input
        | std::views::filter([](char c) { return c != '\n'; })
        | std::ranges::to<std::string>();

    auto height = std::ranges::distance(lines);
    auto width = lines.front().size();

    auto index = [&](auto pos) { return pos.row * width + pos.col; };
    auto from_index = [&](auto i) { return position{
        .row = static_cast<int>(i / width),
        .col = static_cast<int>(i - static_cast<int>(i / width) * width)
    };};
    auto north = [&](auto pos) { return position(pos.row - 1, pos.col); };
    auto south = [&](auto pos) { return position(pos.row + 1, pos.col); };
    auto west = [&](auto pos) { return position(pos.row, pos.col - 1 ); };
    auto east = [&](auto pos) { return position(pos.row, pos.col + 1 ); };

    auto in_bound = [&](auto pos) { return 0 <= pos.row && pos.row < height && 0 <= pos.col && pos.col < width; };
    auto get_pipe = [&](auto pos) {
        return in_bound(pos) ? pipes[index(pos)] : '.';
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

    // ================================= Part 2 =================================

    enum class direction {
        north,
        south,
        west,
        east
    };
    enum class side { none, left, right };

    std::vector<side> sides{pipes.size(), side::none};
    std::queue<size_t> flood_queue;

    auto add_to_queue = [&](position p, side s) {
        auto i = index(p);
        if (graph[i].value() == -1) {
            sides[i] = s;
            flood_queue.push(i);
        }
    };

    auto dir = direction::north;
    auto cur_pos = north(from_index(start));
    auto cur_idx = index(cur_pos);

    while (cur_idx != start) {
        if (cur_idx >= pipes.size())
            std::cout << "fisk";
        if (pipes[cur_idx] == '|') {
            if (dir == direction::north) {
                add_to_queue(west(cur_pos), side::left);
                add_to_queue(east(cur_pos), side::right);
            }
            else if (dir == direction::south) {
                add_to_queue(west(cur_pos), side::right);
                add_to_queue(east(cur_pos), side::left);
            }
        }
        else if (pipes[cur_idx] == '-') {
            if (dir == direction::west) {
                add_to_queue(south(cur_pos), side::left);
                add_to_queue(north(cur_pos), side::right);
            }
            else if (dir == direction::east) {
                add_to_queue(south(cur_pos), side::right);
                add_to_queue(north(cur_pos), side::left);
            }
        }
        else if (pipes[cur_idx] == 'L') {
            if (dir == direction::west) {
                add_to_queue(west(cur_pos), side::left);
                add_to_queue(south(cur_pos), side::left);
                dir = direction::north;
            }
            else if (dir == direction::south) {
                add_to_queue(west(cur_pos), side::right);
                add_to_queue(south(cur_pos), side::right);
                dir = direction::east;
            }
        }
        else if (pipes[cur_idx] == 'J') {
            if (dir == direction::east) {
                add_to_queue(east(cur_pos), side::right);
                add_to_queue(south(cur_pos), side::right);
                dir = direction::north;
            }
            else if (dir == direction::south) {
                add_to_queue(east(cur_pos), side::left);
                add_to_queue(south(cur_pos), side::left);
                dir = direction::west;
            }
        }
        else if (pipes[cur_idx] == '7') {
            if (dir == direction::north) {
                add_to_queue(east(cur_pos), side::right);
                add_to_queue(north(cur_pos), side::right);
                dir = direction::west;
            }
            else if (dir == direction::east) {
                add_to_queue(east(cur_pos), side::left);
                add_to_queue(north(cur_pos), side::left);
                dir = direction::south;
            }
        }
        else if (pipes[cur_idx] == 'F') {
            if (dir == direction::north) {
                add_to_queue(west(cur_pos), side::left);
                add_to_queue(north(cur_pos), side::left);
                dir = direction::east;
            }
            else if (dir == direction::west) {
                add_to_queue(west(cur_pos), side::right);
                add_to_queue(north(cur_pos), side::right);
                dir = direction::south;
            }
        }

        if (dir == direction::north) --cur_pos.row;
        if (dir == direction::south) ++cur_pos.row;
        if (dir == direction::west) --cur_pos.col;
        if (dir == direction::east) ++cur_pos.col;
        cur_idx = index(cur_pos);
    }

    std::vector<bool> visited(pipes.size());
    auto maybe_add = [&](auto pos, side s0) {
        auto i = index(pos);
        if (in_bound(pos) && graph[i].value() == -1) {
            if (s0 == side::right && sides[i] != side::right) {
                sides[i] = side::right;
                flood_queue.push(i);
            }
        }
    };

    while (!flood_queue.empty()) {
        auto front = flood_queue.front();
        auto side = sides[front];
        flood_queue.pop();

        maybe_add(north(from_index(front)), side);
        maybe_add(south(from_index(front)), side);
        maybe_add(west(from_index(front)), side);
        maybe_add(east(from_index(front)), side);
    }

    std::println("Part 2: {}", std::ranges::count_if(sides, [](side s) { return s == side::right; }));

    // Print the loop and fill

    std::ios_base::sync_with_stdio(false);
    std::locale utf8( std::locale(), new std::codecvt_utf8_utf16<wchar_t> );
    std::wcout.imbue(utf8);

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            position pos{row, col};
            auto i = index(pos);
            if (graph[i].value() != -1) {
                wchar_t l = L'a';
                if (pipes[i] == '|') l = L'║';
                if (pipes[i] == '-') l = L'═';
                if (pipes[i] == 'L') l = L'╚';
                if (pipes[i] == 'J') l = L'╝';
                if (pipes[i] == '7') l = L'╗';
                if (pipes[i] == 'F') l = L'╔';
                if (pipes[i] == 'S') l = L'■';

                std::wcout << l;
            }
            else if (sides[i] == side::left)
                std::wcout << L'L';
            else if (sides[i] == side::right)
                std::wcout << L'R';
            else if (pipes[i] == '.')
                std::wcout << L'.';
            else {
                std::wcout << L' ';
            }
        }
        std::wcout << std::endl;
    }
}