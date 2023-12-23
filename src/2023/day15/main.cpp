#include <trak/input.h>
#include <print>
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <numeric>

struct hash {
    u8 operator()(std::string_view str) const {
        u8 current = 0;
        for (char c : str)
            current = ((current + c) * 17);
        return current;
    }
};

int main() {
    using namespace std::literals;

    auto input = trak::read_file("day15/input.input");
    auto seq1 = std::views::split(input, ","sv)
        | std::views::transform([](auto&& rng) { return std::string_view(std::begin(rng), std::end(rng)); })
        | std::views::transform([](auto&& rng) { return hash{}(rng); });

    std::println("--- Day 15: Lens Library ---");
    std::println("Part 1: {}", std::accumulate(std::begin(seq1), std::end(seq1), 0ull));

    // Part 2
    auto seq = std::views::split(input, ","sv)
        | std::views::transform([](auto&& rng) { return std::string_view(std::begin(rng), std::end(rng)); });

    auto hash = ::hash{};
    struct lense { std::string_view label; int length; };
    std::vector<lense> boxes[256];

//    std::unordered_map<std::string_view, int, hash> map;

    for (auto s : seq) {
        auto action = s.find_first_of("-=");
        auto label = s.substr(0, action);

        if (s[action] == '-') {
            auto& box = boxes[hash(label)];
            if (auto it = std::ranges::find_if(box, [&](lense& l) { return l.label == label; }); it != box.end())
                box.erase(it);
        }
        else if (s[action] == '=') {
            auto length = trak::stoi<int>(s.substr(action + 1));
            auto& box = boxes[hash(label)];
            if (auto it = std::ranges::find_if(box, [&](lense& l) { return l.label == label; }); it != box.end())
                *it = { .label = label, .length = length };
            else
                box.push_back({ .label = label, .length = length });
        }
    }

    auto sum = 0;

    for (int i = 0; i < 256; ++i) {
        auto& box = boxes[i];
        if (!box.empty()) {
            for (int j = 0; j < box.size(); ++j)
                sum += (i + 1) * (j + 1) * box[j].length;
        }
    }

    std::println("Part 2: {}", sum);
}