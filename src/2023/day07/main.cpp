#include <trak/input.h>
#include <print>
#include <ranges>
#include <algorithm>
#include <unordered_map>

std::unordered_map<char, char> card_map = {
    {'2', '0'}, {'3', '1'}, {'4', '2'}, {'5', '3'}, {'6', '4'}, {'7', '5'}, {'8', '6'},
    {'9', '7'}, {'T', '8'}, {'J', '9'}, {'Q', 'A'}, {'K', 'B'}, {'A', 'C'}
};

std::unordered_map<char, char> card_map_joker = {
    {'J', '0'}, {'2', '1'}, {'3', '2'}, {'4', '3'}, {'5', '4'}, {'6', '5'}, {'7', '6'},
    {'8', '7'}, {'9', '8'}, {'T', '9'}, {'Q', 'A'}, {'K', 'B'}, {'A', 'C'}
};

enum class hand_type : int {
    high_card = 0,
    one_pair = 1,
    two_pairs = 2,
    three_of_kind = 3,
    full_house = 4,
    four_of_kind = 5,
    five_of_kind = 6
};

struct hand {
    hand_type type;
    std::string cards;
    int bid;
};

hand_type map_type(int count, hand_type s) {
    if (count == 5) {
        return hand_type::five_of_kind;
    }
    else if (count == 4) {
        return hand_type::four_of_kind;
    }
    else if (count == 3) {
        if (s == hand_type::high_card)
            return hand_type::three_of_kind;
        else if (s == hand_type::one_pair)
            return hand_type::full_house;
    }
    else if (count == 2) {
        if (s == hand_type::high_card)
            return hand_type::one_pair;
        if (s == hand_type::one_pair)
            return hand_type::two_pairs;
        if (s == hand_type::three_of_kind)
            return hand_type::full_house;
    }

    return s;
}

template<typename It>
hand_type get_hand_type(It prev, It end) {
    auto s = hand_type::high_card;
    int count = 1;
    for (auto cur = prev + 1; cur < end; ++cur) {
        if (*prev != *cur) {
            s = map_type(count, s);
            prev = cur;
            count = 0;
        }
        ++count;
    }
    return map_type(count, s);
}

hand_type get_hand_type(std::string_view card_view) {
    std::string cards(card_view);
    std::sort(cards.begin(), cards.end());
    return get_hand_type(cards.begin(), cards.end());
}

hand_type get_hand_type_with_jokers(std::string_view card_view) {
    std::string cards(card_view);
    std::sort(cards.begin(), cards.end(), [&](char a, char b) { return card_map_joker.at(a) < card_map_joker.at(b); });

    int jokers = 0;
    auto prev = cards.begin();
    while (prev != cards.end() && *prev == 'J') {
        ++jokers;
        ++prev;
    }

    if (jokers == 5 || jokers == 4)
        return hand_type::five_of_kind;

    auto s = get_hand_type(prev, cards.end());

    if (jokers == 3) {
        if (s == hand_type::one_pair)
            return hand_type::five_of_kind;
        return hand_type::four_of_kind;
    }
    if (jokers == 2) {
        if (s == hand_type::three_of_kind)
            return hand_type::five_of_kind;
        if (s == hand_type::one_pair)
            return hand_type::four_of_kind;
        return hand_type::three_of_kind;
    }
    if (jokers == 1) {
        if (s == hand_type::four_of_kind)
            return hand_type::five_of_kind;
        if (s == hand_type::three_of_kind)
            return hand_type::four_of_kind;
        if (s == hand_type::two_pairs)
            return hand_type::full_house;
        if (s == hand_type::one_pair)
            return hand_type::three_of_kind;
        return hand_type::one_pair;
    }

    return s;
}

bool compare_hands(const hand& a, const hand& b) {
    return a.type == b.type
        ? a.cards < b.cards
        : a.type < b.type;
}

int main()
{
    auto input = trak::read_file("day07/input.input");
    auto hands = std::views::split(input, '\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& line) { return std::string_view(line); })
        | std::views::transform([&](std::string_view line) -> hand {
            return {
                .type = get_hand_type(line.substr(0, 5)),
                .cards = line.substr(0, 5)
                         | std::views::transform([&](char c) { return card_map.at(c); })
                         | std::ranges::to<std::string>(),
                .bid = trak::stoi<int>(line.substr(5)) };
        })
        | std::ranges::to<std::vector>();
    std::ranges::sort(hands, compare_hands);

    std::println("--- Day 7: Camel Cards ---");
    int rank = 1;
    std::println("Part 1: {}",
        std::ranges::fold_left(hands | std::views::transform([&rank](const hand& h) {
            return rank++ * h.bid; }), 0, std::plus()));

    hands = std::views::split(input, '\n')
        | std::views::filter([](auto&& rng) { return !std::empty(rng); })
        | std::views::transform([](auto&& line) { return std::string_view(line); })
        | std::views::transform([&](std::string_view line) -> hand {
            return {
                .type = get_hand_type_with_jokers(line.substr(0, 5)),
                .cards = line.substr(0, 5)
                         | std::views::transform([&](char c) { return card_map_joker.at(c); })
                         | std::ranges::to<std::string>(),
                .bid = trak::stoi<int>(line.substr(5)) };
        })
        | std::ranges::to<std::vector>();
    std::ranges::sort(hands, compare_hands);

    rank = 1;
    std::println("Part 2: {}",
        std::ranges::fold_left(hands | std::views::transform([&rank](const hand& h) {
            return rank++ * h.bid; }), 0, std::plus()));
}