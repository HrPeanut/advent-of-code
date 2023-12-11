#include <trak/input.h>
#include <print>
#include <sstream>
#include <ranges>
#include <algorithm>

struct game {
    int id;
    int red_cubes = 0;
    int green_cubes = 0;
    int blue_cubes = 0;
};

game parse_game(std::string_view line) {
    game g{};
    g.id = std::stoi(line.data() + 5);
    for (auto&& x : line.substr(line.find(':') + 1, line.size()) | std::views::split(';')) {
        for (auto&& y: x | std::views::split(',')) {
            std::string_view str_view(y);
            if (str_view.ends_with("red"))
                g.red_cubes = std::max(g.red_cubes, std::stoi(str_view.data()));
            else if (str_view.ends_with("green"))
                g.green_cubes = std::max(g.green_cubes, std::stoi(str_view.data()));
            else if (str_view.ends_with("blue"))
                g.blue_cubes = std::max(g.blue_cubes, std::stoi(str_view.data()));
        }
    }
    return g;
}

int main()
{
    auto input = trak::read_file("day02/input.input");

    std::println("--- Day 2: Cube Conundrum ---");
    std::println("Part 1: {}",
        std::ranges::fold_left(
            input
            | std::views::split('\n')
            | std::views::transform([](auto&& line) {
                return parse_game(std::string_view(line));
            })
            | std::views::transform([](game&& game) -> int {
                return game.red_cubes <= 12 && game.green_cubes <= 13 && game.blue_cubes <= 14 ? game.id : 0;
            }), 0, std::plus<>()));
    std::println("Part 2: {}",
        std::ranges::fold_left(
            input
            | std::views::split('\n')
            | std::views::transform([](auto&& line) {
                return parse_game(std::string_view(line));
            })
            | std::views::transform([](game&& game) -> int {
                return game.red_cubes * game.green_cubes * game.blue_cubes;
            }), 0, std::plus<>()));
}