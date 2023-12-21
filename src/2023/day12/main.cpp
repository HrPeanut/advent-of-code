#include <trak/input.h>
#include <print>
#include <ranges>
#include <algorithm>
#include <unordered_map>

struct pair_hash {
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};

using count_type = int64_t;
using cache_map = std::unordered_map<std::pair<std::string_view, int*>, count_type, pair_hash>;

template<typename It>
auto count_arrangements(It it, It end, std::string_view conditions, cache_map& cache, int i = 0) -> int64_t {
    count_type count = 0;
    // If we have no more rules, then this is a valid arrangement if conditions contains no '#' (since then we need more rules to match it)
    if (it == end) {
        return conditions.find('#') == std::string_view::npos ? 1 : 0;
    } else if (conditions.empty())
//        return 0;
        return it == end ? 1 : 0;

    if (cache.contains({ conditions, &*it }))
        return cache[{ conditions, &*it }];

    auto rule = *it;
    auto c = conditions[0];

    // If '.' we cannot place the rule here and if '?', count_arrangements arrangements if we consider the '?' as '.' (aka skipping this pos)
    if (c == '.' || c == '?') {
        count += count_arrangements(it, end, conditions.substr(1), cache, i);
    }

    // If the current rule is longer that the remaining conditions, there is no more possible arrangements
    if (conditions.size() < rule)
        return count;

    // If '#' or '?' count_arrangements arrangements when we place the current rule here.
    if (c == '#' || c == '?') {
        // Check if we can actually place the current rule here. I.e.
        // 1. There must be at least rule number of '#' or '?'
        // 2. The rule must not be succeeded by an '#' (if so, the rule does not fit)
        if (conditions.find('.') >= rule) {
            if (rule == conditions.size())
                // Consume the rule and continue with an empty view.
                count += count_arrangements(std::next(it), end, std::string_view(), cache);
            else if (conditions[rule] != '#')
                // Consume the rule and skip rule chars.
                count += count_arrangements(std::next(it), end, conditions.substr(rule + 1), cache);
        }
    }

    cache[{ conditions, &*it }] = count;
    return count;
}

auto arrangements(std::string_view conditions, std::vector<int> rules) {
    cache_map cache;
    count_arrangements(rules.begin(), rules.end(), conditions, cache);
    return cache[{ conditions, &*rules.begin() }];
}

auto part_one(std::string_view line) {
    auto space = line.find(' ');
    auto conditions = line.substr(0, space);
    auto rules = line.substr(space)
        | std::views::split(',')
        | std::views::transform([](auto&& rng) { return trak::stoi<int>(std::string_view(rng)); })
        | std::ranges::to<std::vector>();

    return arrangements(conditions, rules);
}

auto part_two(std::string_view line) {
    auto repeat = [](std::string_view view, char delim) {
        return std::views::repeat(view, 5)
            | std::views::join_with(delim)
            | std::ranges::to<std::string>();
    };

    auto space = line.find(' ');
    auto conditions = repeat(line.substr(0, space), '?');
    auto rules = repeat(line.substr(space + 1), ',')
        | std::views::split(',')
        | std::views::transform([](auto&& rng) { return trak::stoi<int>(std::string_view(rng)); })
        | std::ranges::to<std::vector>();
    return arrangements(conditions, rules);
}

int main() {
    auto input = trak::read_file("day12/input.input");
    auto lines = std::views::split(input, '\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& rng) { return std::string_view(rng); });

    std::println("--- Day 12: Hot Springs ---");
    std::println("Part 1: {}",
        std::ranges::fold_left(lines, 0, [](int64_t acc, auto line) { return acc + part_one(line); }));

    std::println("Part 2: {}",
        std::ranges::fold_left(lines, 0, [](int64_t acc, auto line) { return acc + part_two(line); }));
}