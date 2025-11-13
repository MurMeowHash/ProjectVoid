#pragma once
#include <string>
#include <nlohmann/json.hpp>

class GameObject;
class ObjectComponent;

namespace SceneDeserializer {
    nlohmann::json ReadJsonFile(const std::string &filePath);
    void DeserializeScene(const nlohmann::json &scene);
}
