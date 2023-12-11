#ifndef TRAK_INPUT_H
#define TRAK_INPUT_H

#include <string>
#include <fstream>
#include <vector>

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
}

#endif //TRAK_INPUT_H
