#pragma once
#include <string>

#define NODISCARD [[nodiscard]]
#define UNUSED [[maybe_unused]]

namespace Utils {
    std::string ReadTextFromFile(const std::string &filePath);
}