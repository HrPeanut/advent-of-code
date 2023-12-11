#include <trak/input.h>
#include <print>
#include <ranges>
#include <unordered_map>

struct grid_type {
    size_t width{};
    size_t height{};
    std::vector<std::string_view> data;
};

char get_bounded(const grid_type& grid, int row, int col) {
    if (row < 0 || row >= grid.height || col < 0 || col >= grid.width)
        return '.';
    return grid.data[row][col];
}

template<typename F> requires std::invocable<F, int, int, int>
void for_each_number(const grid_type& grid, F callback) {
    for (int row = 0; row < grid.width; ++row) {
        for (int col = 0; col < grid.height; ++col) {
            if (std::isdigit(get_bounded(grid, row, col))) {
                int end = col;

                while (std::isdigit(get_bounded(grid, row, end)))
                    ++end;

                callback(row, col, end);
                col = end;
            }
        }
    }
}

bool adjacent(const grid_type& grid, int row, int col_begin, int col_end) {
    for (int y = row - 1; y <= row + 1; ++y) {
        for (int x = col_begin - 1; x <= col_end; ++x) {
            auto c = get_bounded(grid, y, x);
            if (c != '.' && !std::isdigit(c))
                return true;
        }
    }

    return false;
}

int run_part_1(const grid_type& grid) {
    int sum = 0;
    for_each_number(grid, [&](int row, int col_begin, int col_end) {
        if (adjacent(grid, row, col_begin, col_end))
            sum += std::stoi(grid.data[row].data() + col_begin);
    });
    return sum;
}

int run_part_2(const grid_type& grid) {
    std::unordered_map<uint64_t, std::vector<int>> gears;
    for_each_number(grid, [&](int row, int col_begin, int col_end) {
        for (int y = row - 1; y <= row + 1; ++y) {
            for (int x = col_begin - 1; x <= col_end; ++x) {
                auto c = get_bounded(grid, y, x);
                if (c == '*') {
                    auto key = static_cast<uint64_t>(y) << 32 | static_cast<uint64_t>(x);
                    gears[key].push_back(std::stoi(grid.data[row].data() + col_begin));
                }
            }
        }
    });

    int sum = 0;
    for (const auto& [_, gear] : gears) {
        if (gear.size() == 2) {
            sum += gear[0] * gear[1];
        }
    }

    return sum;
}

int main()
{
    grid_type grid;

    auto input = trak::read_file("day03/input.input");
    grid.data = input
        | std::views::split('\n')
        | std::views::transform([](auto&& x) { return std::string_view(x); })
        | std::ranges::to<std::vector>();
    grid.width = grid.data[0].size();
    grid.height = grid.data.size();

    std::println("--- Day 3: Gear Ratios ---");
    std::println("Part 1: {}", run_part_1(grid));
    std::println("Part 2: {}", run_part_2(grid));
}