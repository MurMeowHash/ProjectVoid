#include "Shader.h"
#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "../Error/Error.h"
#include "../Utils/Utils.h"

Shader::Shader() : id(UNINITIALIZED) {
}

int Shader::CheckCompileErrors(const GLuint shader, const std::string &type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            Error::FallWithFatalError("Shader", "Compilation failed", infoLog);
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            Error::FallWithFatalError("Shader", "Linking failed", infoLog);
        }
    }
    return success;
}

GLuint Shader::CompileShader(const std::string &shaderPath, const GLuint type, const std::string &name) {
    const std::string shaderSource = Utils::ReadTextFromFile(shaderPath);
    const char *shaderCode = shaderSource.c_str();
    const GLuint vertShader = glCreateShader(type);
    glShaderSource(vertShader, 1, &shaderCode, nullptr);
    glCompileShader(vertShader);
    CheckCompileErrors(vertShader, name);
    return vertShader;
}

void Shader::Load(const char *vertPath, const char *fragPath) {
    const GLuint vertShader = CompileShader(vertPath, GL_VERTEX_SHADER, "VERTEX");
    const GLuint fragShader = CompileShader(fragPath, GL_FRAGMENT_SHADER, "FRAGMENT");

    const GLuint idTemp = glCreateProgram();
    glAttachShader(idTemp, vertShader);
    glAttachShader(idTemp, fragShader);
    glLinkProgram(idTemp);

    if (CheckCompileErrors(idTemp, "PROGRAM")) {
        if (id != UNINITIALIZED) {
            glDeleteProgram(id);
        }
        id = idTemp;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void Shader::Load(const char *vertPath, const char *fragPath, const char *geomPath) {
    const GLuint vertShader = CompileShader(vertPath, GL_VERTEX_SHADER, "VERTEX");
    const GLuint fragShader = CompileShader(fragPath, GL_FRAGMENT_SHADER, "FRAGMENT");
    const GLuint geomShader = CompileShader(fragPath, GL_GEOMETRY_SHADER, "GEOMETRY");

    const GLuint idTemp = glCreateProgram();
    glAttachShader(idTemp, vertShader);
    glAttachShader(idTemp, fragShader);
    glAttachShader(idTemp, geomShader);
    glLinkProgram(idTemp);

    if (CheckCompileErrors(idTemp, "PROGRAM")) {
        if (id != UNINITIALIZED) {
            glDeleteProgram(id);
        }
        id = idTemp;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    glDeleteShader(geomShader);
}

void Shader::Bind() const {
    glUseProgram(id);
}

void Shader::SetInt(const char *name, const int value) const {
    glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::SetFloat(const char *name, const float value) const {
    glUniform1f(glGetUniformLocation(id, name), value);
}

void Shader::SetVec2(const char *name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(id, name), 1, value_ptr(value));
}

void Shader::SetVec3(const char *name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(id, name), 1, value_ptr(value));
}

void Shader::SetVec4(const char *name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(id, name), 1, value_ptr(value));
}

void Shader::SetMat3(const char *name, const glm::mat3 &value) const {
    glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, value_ptr(value));
}

void Shader::SetMat4(const char *name, const glm::mat4 &value) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, value_ptr(value));
}

void Shader::DeleteShader() const {
    glDeleteProgram(id);
}