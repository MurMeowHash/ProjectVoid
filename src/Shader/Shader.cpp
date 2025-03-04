#include "Shader.h"
#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "../Utils/Utils.h"

Shader::Shader() : id(-1) {
}

Shader::~Shader() {
    DeleteShader();
}

int Shader::CheckCompileErrors(const GLuint shader, const std::string &type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            // Error system
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << '\n' << infoLog << '\n';
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            // Error system
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << '\n' << infoLog << '\n';
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
        if (id != -1) {
            glDeleteProgram(id);
        }
        id = idTemp;
    }
    else {
        // Error system
        std::cout << "shader failed to compile " << vertPath << ' ' << fragPath << '\n';
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
        if (id != -1) {
            glDeleteProgram(id);
        }
        id = idTemp;
    }
    else {
        // Error system
        std::cout << "shader failed to compile " << vertPath << ' ' << fragPath << ' ' << geomPath << '\n';
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void Shader::Use() const {
    glUseProgram(id);
}

void Shader::SetBool(const char *name, const bool value) const {
    glUniform1i(glGetUniformLocation(id, name), value);
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

void Shader::SetMat4(const char *name, const glm::mat4 &value) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, value_ptr(value));
}

void Shader::DeleteShader() const {
    glDeleteProgram(id);
}