#ifndef TRAK_INPUT_H
#define TRAK_INPUT_H

#include <string>
#include <fstream>
#include <vector>
#include <charconv>

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

namespace trak {

    std::string read_file(std::string_view filename) {
        constexpr size_t buffer_size = 8192;
        char file_buffer[buffer_size];

        std::ifstream file;
        file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        file.rdbuf()->pubsetbuf(file_buffer, buffer_size);
        file.open(filename.data(), std::ios::binary | std::ios::ate);

        auto size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string content(size, '\0');
        file.read(content.data(), size);

        return content;
    }

    template<typename T>
    T stoi(std::string_view view) {
        T result;

        auto begin = view.data();
        auto end = begin + view.size();
        while (begin != end && *begin == ' ')
            ++begin;

        std::from_chars(begin, end, result);
        return result;
    }

    template<typename T>
    auto transpose(const std::vector<T>& grid) {
        std::vector<std::string> transpose(grid.front().size(), std::string(grid.size(), '-'));
        for (std::vector<int>::size_type i = 0; i < grid[0].size(); i++)
            for (std::vector<int>::size_type j = 0; j < grid.size(); j++) {
                transpose[i][j] = grid[j][i];

        }
        return transpose;
    }
}

#endif //TRAK_INPUT_H
