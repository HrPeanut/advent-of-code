#include <trak/input.h>
#include <print>
#include <iostream>
#include <ranges>
#include <unordered_map>

using grid_type = std::vector<std::string>;

struct grid_hash {
    size_t operator()(const grid_type& grid) const {
        auto hash = std::hash<std::string>();
        std::size_t seed = grid.size();
        for(auto& i : grid) {
            seed ^= hash(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

auto move_north(grid_type& grid) {
    for (auto& p : grid) {
        int next_pos = 0;

        for (int i = 0; i < p.size(); ++i) {
            if (p[i] == 'O') {
                p[i] = '.';
                p[next_pos++] = 'O';
            }
            if (p[i] == '#')
                next_pos = i + 1;
        }
    }
}

auto count_load(const grid_type& grid) {
    u64 load = 0;

    for (auto& p : grid) {
        for (int i = 0; i < p.size(); ++i) {
            if (p[i] == 'O') {
                load += p.size() - i;
            }
        }
    }

    return load;
}

auto print_grid(const grid_type& grid) {
    std::cout << "--- Load " << count_load(grid) << " ---\n";
    auto transpose = trak::transpose(grid);
    auto idx = grid.size();
    for (auto& line : transpose)
        std::cout << line << ' ' << idx-- << std::endl;
}

auto rotate(const grid_type& grid) {
    auto size = grid.size();
    std::vector<std::string> rotated(grid.front().size(), std::string(grid.size(), '-'));

    for (size_t row = 0; row < size; ++row) {
        for (size_t col = 0; col < size; ++col) {
            rotated[size - 1 - col][row] = grid[row][col];
        }
    }

    return rotated;
}

auto cycle(grid_type grid) {
    for (int i = 0; i < 4; ++i) {
        move_north(grid);
        grid = rotate(grid);
    }

//    print_grid(grid);
    return grid;
}

int main() {
    using namespace std::literals;

    auto input = trak::read_file("day14/input.input");
    auto lines = std::views::split(input, "\n"sv)
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& rng) { return std::string(std::begin(rng), std::end(rng)); })
        | std::ranges::to<std::vector>();

    std::println("--- Day 14: Parabolic Reflector Dish ---");
    auto grid = trak::transpose(lines);
    move_north(grid);
    std::println("Part 1: {}", count_load(grid));

    // Part 2
    grid = trak::transpose(lines);
    u64 load = 0;
    std::vector<u64> loads;
    std::unordered_map<grid_type, u64, grid_hash> map;

    const u64 max_cycles = 1000000000;
    for (u64 i = 0; i < max_cycles; ++i) {
        grid = cycle(grid);

        if (map.contains(grid)) {
            auto j = map[grid];
            auto final = (max_cycles - j) % (loads.size() - j) + j - 1;
            load = loads[final];
            break;
        }

        map[grid] = i;
        loads.push_back(count_load(grid));
    }

    std::println("Part 2: {}", load);
}