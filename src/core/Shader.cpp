// ====================================================================
// FILE:     src/core/Shader.cpp
// OWNER:    Member 1
// TASKS:    M1-01
// STATUS:   IMPLEMENTED
// ====================================================================

#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() : ID(0) {}

Shader::~Shader() {
    if (ID != 0) {
        glDeleteProgram(ID);
        ID = 0;
    }
}

std::string Shader::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[Shader] Error: Could not open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source, const std::string& stageName) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "[Shader] Error: Compilation failed for " << stageName << ":\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::load(const std::string& vertPath, const std::string& fragPath) {
    std::string vertCode = readFile(vertPath);
    std::string fragCode = readFile(fragPath);

    if (vertCode.empty() || fragCode.empty()) {
        return false;
    }

    unsigned int vertShader = compileShader(GL_VERTEX_SHADER, vertCode, "VERTEX");
    unsigned int fragShader = compileShader(GL_FRAGMENT_SHADER, fragCode, "FRAGMENT");

    if (vertShader == 0 || fragShader == 0) {
        return false;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertShader);
    glAttachShader(ID, fragShader);
    glLinkProgram(ID);

    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(ID, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "[Shader] Error: Program linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(ID);
        ID = 0;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return ID != 0;
}

void Shader::use() const {
    if (ID != 0) {
        glUseProgram(ID);
    }
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
