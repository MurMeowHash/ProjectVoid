#include "Utils.h"
#include <fstream>

std::string ReadTextFromFile(const std::string &filePath) {
    std::ifstream file(filePath);
    std::string text;
    std::string line;
    while (std::getline(file, line)) {
        text += line + '\n';
    }
    return text;
}