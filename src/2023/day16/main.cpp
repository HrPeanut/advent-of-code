#include <trak/input.h>
#include <print>
#include <ranges>
#include <algorithm>
#include <unordered_set>
#include <queue>
#include <complex>

using namespace std::complex_literals;
using complex = std::complex<int>;
using pos_dir = std::pair<complex, complex>;

complex operator+(int a, complex b) { return complex(a) + b; }
complex operator+(complex a, std::complex<double> b) { return a + complex((int)b.real(), (int)b.imag()); }
complex operator-(complex a, std::complex<double> b) { return a - complex((int)b.real(), (int)b.imag()); }

enum tile_type : u8 {
    empty = 0,
    forward_mirror = 1,
    backward_mirror = 2,
    vertical_splitter = 4,
    horizontal_splitter = 5
};

enum direction_type : u8 {
    direction_up = (1 << 4),
    direction_down = (1 << 5),
    direction_left = (1 << 6),
    direction_right = (1 << 7)
};

u64 width;
u64 height;

inline u64 index(complex pos) { return pos.imag() * width + pos.real(); }
inline u8 visited(u8 val) { return (val >> 4); }
inline u8 direction(complex dir) {
    if (dir.real() == 1) return direction_right;
    if (dir.real() == -1) return direction_left;
    if (dir.imag() == 1) return direction_down;
    if (dir.imag() == -1) return direction_up;
    std::unreachable();
}

u64 energized(std::vector<u8> grid, complex pos, complex dir) {
    std::queue<pos_dir> queue;
    queue.emplace(pos, dir);

    while (!queue.empty()) {
        std::tie(pos, dir) = queue.front();
        queue.pop();

        if (pos.real() < 0 || pos.real() >= width || pos.imag() < 0 || pos.imag() >= height)
            continue;

        auto i = index(pos);
        auto cur = grid[i];
        if (cur & direction(dir))
            continue;
        grid[i] |= direction(dir);

        switch (cur & 0xF) {
            case vertical_splitter:
                queue.emplace(pos + 1i, 1i);
                queue.emplace(pos - 1i, -1i);
                break;
            case horizontal_splitter:
                queue.emplace(pos + 1, 1);
                queue.emplace(pos - 1, -1);
                break;
            case forward_mirror:
                dir = -complex(dir.imag(), dir.real());
                queue.emplace(pos + dir, dir);
                break;
            case backward_mirror:
                dir = complex(dir.imag(), dir.real());
                queue.emplace(pos + dir, dir);
                break;
            default:
                queue.emplace(pos + dir, dir);
                break;
        }
    }

    return std::count_if(grid.begin(), grid.end(), [](u8 val) { return visited(val); });
}

int main() {
    auto input = trak::read_file("day16/input.input");
    width = input.find('\n');
    height = input.size() / width;

    auto grid = std::views::split(input, '\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& rng) {
            return rng | std::views::transform([](char c) -> u8 {
                switch (c) {
                    case '|': return tile_type::vertical_splitter;
                    case '-': return tile_type::horizontal_splitter;
                    case '/': return tile_type::forward_mirror;
                    case '\\': return tile_type::backward_mirror;
                    default: return tile_type::empty;
                }
            });
        })
        | std::views::join
        | std::ranges::to<std::vector>();

    std::println("--- Day 16: The Floor Will Be Lava ---");
    std::println("Part 1: {}", energized(grid, 0, 1));

    std::vector<u64> energized_tiles(width * 2 + height * 2);
    for (int i = 0; i < width; ++i) {
        energized_tiles.emplace_back(energized(grid, i, 1i));
        energized_tiles.emplace_back(energized(grid, complex(i, (int)height - 1), -1i));
    }
    for (int i = 0; i < height; ++i) {
        energized_tiles.emplace_back(energized(grid, complex(0, i), 1));
        energized_tiles.emplace_back(energized(grid, complex((int)width - 1, i), -1));
    }

    std::println("Part 2: {}", *std::ranges::max_element(energized_tiles));
}