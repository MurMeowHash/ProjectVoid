#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace SceneSerializer {
    nlohmann::json SerializeScene();
    void WriteJsonFile(const std::string &filePath, const nlohmann::json &data);
    void SerializeSceneToFile(const std::string &filePath);
}

