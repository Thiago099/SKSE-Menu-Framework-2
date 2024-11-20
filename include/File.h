#pragma once

namespace File {
    inline bool Exists(const wchar_t* filename) {
        std::ifstream file(filename);
        return file.good();
    }
}