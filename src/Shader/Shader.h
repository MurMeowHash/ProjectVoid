#pragma once
#include <glad.h>
#include <string>
#include <glm/glm.hpp>

class Shader {
private:
    static constexpr int UNINITIALIZED = -1;
    GLuint id;
    static int CheckCompileErrors(GLuint shader, const std::string &type);
    static GLuint CompileShader(const std::string &shaderPath, GLuint type, const std::string &name);
public:
    Shader();
    void Load(const char *vertPath, const char* fragPath);
    void Load(const char *vertPath, const char* fragPath, const char* geomPath);
    void Bind() const;
    void SetInt(const char *name, int value) const;
    void SetFloat(const char *name, float value) const;
    void SetVec2(const char *name, const glm::vec2& value) const;
    void SetVec3(const char *name, const glm::vec3& value) const;
    void SetVec4(const char *name, const glm::vec4& value) const;
    void SetMat3(const char *name, const glm::mat3& value) const;
    void SetMat4(const char *name, const glm::mat4& value) const;
    void DeleteShader() const;
};
