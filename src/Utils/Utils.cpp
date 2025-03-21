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

    GLenum GetSourceFromDesiredFormat(TextureFormat desiredFormat) {
        GLenum srcFormat;
        switch (desiredFormat) {
            case TextureFormat::RGB:
            case TextureFormat::RGB16F:
            case TextureFormat::RGB32F:
            case TextureFormat::SRGB:
                srcFormat = GL_RGB;
                break;
            case TextureFormat::RGBA:
            case TextureFormat::RGBA16F:
            case TextureFormat::RGBA32F:
            case TextureFormat::SRGBA:
                srcFormat = GL_RGBA;
                break;
            case TextureFormat::DepthStencil:
                srcFormat = GL_DEPTH_STENCIL;
                break;
            default:
                srcFormat = static_cast<GLenum>(desiredFormat);
        }

        return srcFormat;
    }
}