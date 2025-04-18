#include "Utils.h"
#include <fstream>
#include "../Game/Types/CreateParameters.h"
#include <sstream>

namespace Utils {
    size_t FindNameBeginning(const std::string &path);

    size_t FindNameBeginning(const std::string &path) {
        auto lastSlashPos = FindLastSlash(path);
        return lastSlashPos == std::string::npos ? 0 : lastSlashPos + 1;
    }

	std::string ReadTextFromFile(const std::string &filePath) {
    	std::ifstream file(filePath);
        std::stringstream fileContentStream;
        fileContentStream << file.rdbuf();
        std::string fileContent = fileContentStream.str();
        file.close();
        return fileContent;
	}

    glm::vec3 ToGLMVector(const aiVector3D &target) {
        return {target.x, target.y, target.z};
    }

    size_t FindLastSlash(const std::string &path) {
        auto lastSlashPos = path.find_last_of('/');
        if(lastSlashPos == std::string::npos) {
            lastSlashPos = path.find_last_of('\\');
        }

        return lastSlashPos;
    }

    std::string GetExtendedNameFromPath(const std::string &path) {
        return path.substr(FindNameBeginning(path));
    }

    std::string GetNameFromPath(const std::string &path) {
        auto nameBeginning = FindNameBeginning(path);
        auto extensionPos = path.find_last_of('.');
        return path.substr(nameBeginning, extensionPos - nameBeginning);
    }

    std::string GetExtensionFromPath(const std::string &path) {
        return path.substr(path.find_last_of('.') + 1);
    }

    BufferFormat GetSourceFromDesiredFormat(BufferFormat desiredFormat) {
        switch (desiredFormat) {
            case BufferFormat::RGB:
            case BufferFormat::RGB16F:
            case BufferFormat::RGB32F:
            case BufferFormat::SRGB:
                return BufferFormat::RGB;
            case BufferFormat::RGBA:
            case BufferFormat::RGBA16F:
            case BufferFormat::RGBA32F:
            case BufferFormat::SRGBA:
                return BufferFormat::RGBA;
            default:
                return desiredFormat;
        }
    }

    TextureDataType GetTextureDataTypeFromFormat(BufferFormat format) {
        switch (format) {
            case BufferFormat::RGB:
            case BufferFormat::RGBA:
            case BufferFormat::SRGB:
            case BufferFormat::SRGBA:
                return TextureDataType::Byte;
            default:
                return TextureDataType::Float;
        }
    }
}