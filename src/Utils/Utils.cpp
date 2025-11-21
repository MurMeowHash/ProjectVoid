#include "Utils.h"
#include <fstream>
#include "../Game/Types/CreateParameters.h"
#include <sstream>
#include "../Game/Types/GLBuffer/FrameBuffer/FrameBuffer.h"
#include <stb/stb_image_write.h>
#include "../Game/ComponentScripts/Camera/Camera.h"
#include <imgui/imgui.h>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#ifdef CreateWindow
#undef CreateWindow
#endif

namespace Utils {
    constexpr GLsizeiptr MB_TO_B_MULTIPLIER = 1024 * 1024;
    constexpr GLsizeiptr KB_TO_B_MULTIPLIER = 1024;

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

    glm::vec3 ToGLMVector(const btVector3 &target) {
        return {target.x(), target.y(), target.z()};
    }

    btVector3 ToBulletVector(const glm::vec3 &target) {
        return {target.x, target.y, target.z};
    }

    Transform ToEngineTransform(const btTransform &transform) {
        Transform convertedTransform;
        convertedTransform.position = ToGLMVector(transform.getOrigin());
        auto quatRotation = transform.getRotation();
        quatRotation.getEulerZYX(convertedTransform.rotation.z, convertedTransform.rotation.y, convertedTransform.rotation.x);
        convertedTransform.rotation = glm::degrees(convertedTransform.rotation);
        return convertedTransform;
    }

    btTransform ToBulletTransform(const Transform &transform) {
        btTransform bulletTransform;
        bulletTransform.setIdentity();
        bulletTransform.setOrigin(ToBulletVector(transform.position));
        btQuaternion bulletRotation;
        bulletRotation.setEulerZYX(glm::radians(transform.rotation.z), glm::radians(transform.rotation.y),
                                   glm::radians(transform.rotation.x));
        bulletTransform.setRotation(bulletRotation);
        return bulletTransform;
    }

    size_t FindLastSlash(const std::string &path) {
        auto lastSlashPos = path.find_last_of('/');
        if(lastSlashPos == std::string::npos) {
            lastSlashPos = path.find_last_of('\\');
        }

        return lastSlashPos;
    }

    std::string NormalizePath(const std::string &path) {
        std::string normalized = path;
        if(!normalized.empty() && (normalized.back() == '\\' || normalized.back() == '/')) {
            normalized.pop_back();
        }
        return normalized;
    }

    int FindOptimalTextLength(const std::string &text, float availableWidth) {
        int left = 0;
        int right = static_cast<int>(text.length());
        int bestLength = 0;
        
        while(left <= right) {
            const int mid = (left + right) / 2;
            std::string testName = text.substr(0, mid);
            const float testWidth = ImGui::CalcTextSize(testName.c_str()).x;
            
            if(testWidth <= availableWidth) {
                bestLength = mid;
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        
        return bestLength;
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

    GLsizeiptr KBToB(GLsizeiptr kb) {
        return kb * KB_TO_B_MULTIPLIER;
    }

    GLsizeiptr MBToB(GLsizeiptr mb) {
        return mb * MB_TO_B_MULTIPLIER;
    }

    void SaveFrameBufferToJPG(const char *fileName, const FrameBuffer &buf, GLenum colorAttachment, int quality) {
        GLsizei bufWidth = buf.GetWidth();
        GLsizei bufHeight = buf.GetHeight();
        auto *bufData = new unsigned char[bufWidth * bufHeight * 3];
        GLint prevFrameBuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, buf.GetHandle());
        glReadBuffer(colorAttachment);
        glReadPixels(0, 0, bufWidth, bufHeight, GL_RGB, GL_UNSIGNED_BYTE, bufData);
        stbi_flip_vertically_on_write(true);
        stbi_write_jpg(fileName, bufWidth, bufHeight, 3, bufData, quality);
        delete [] bufData;
        glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);
    }

    glm::vec3 GetTranslationFromMatrix(const glm::mat4 &matrix) {
        return {matrix[3][0], matrix[3][1], matrix[3][2]};
    }

    glm::vec3 GetScaleFromMatrix(const glm::mat4 &matrix) {
        glm::vec3 scale;
        scale.x = glm::length(glm::vec3(matrix[0]));
        scale.y = glm::length(glm::vec3(matrix[1]));
        scale.z = glm::length(glm::vec3(matrix[2]));
        return scale;
    }

    glm::bvec3 InvertVector3(const glm::bvec3 &vec) {
        return {!vec.x, !vec.y, !vec.z};
    }

    glm::vec3 NDCToWorld(const glm::vec4 &ndcCoord, const Camera &cam) {
        glm::mat4 viewProjInverse = glm::inverse(cam.GetViewMatrix() * cam.GetProjectionMatrix());
        glm::vec4 rawWorldCoord = viewProjInverse * ndcCoord;
        return {rawWorldCoord / rawWorldCoord.w};
    }

    void OpenInExplorer(const std::string &path) {
        if(!std::filesystem::exists(path)) {
            return;
        }
        
        std::string pathStr = path;
        std::ranges::replace(pathStr, '/', '\\');
        
        const int widePathSize = MultiByteToWideChar(CP_UTF8, 0, pathStr.c_str(), -1, nullptr, 0);
        std::vector<wchar_t> widePath(widePathSize);
        MultiByteToWideChar(CP_UTF8, 0, pathStr.c_str(), -1, widePath.data(), widePathSize);
        
        if(std::filesystem::is_directory(path)) {
            ShellExecuteW(nullptr, L"open", L"explorer.exe", widePath.data(), nullptr, SW_SHOWDEFAULT);
        } else {
            ShellExecuteW(nullptr, L"open", widePath.data(), nullptr, nullptr, SW_SHOWDEFAULT);
        }
    }

    bool IsImageFile(const std::filesystem::path& path) {
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        return extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
               extension == ".bmp" || extension == ".gif" || extension == ".tga" ||
               extension == ".tiff" || extension == ".webp";
    }
}