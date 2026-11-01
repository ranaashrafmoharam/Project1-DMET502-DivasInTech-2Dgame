#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::~Shader() {
    if (id != 0) {
        glDeleteProgram(id);
        id = 0;
    }
}

GLuint Shader::compile(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compile error (" 
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") 
                  << "): " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::loadFromFiles(const std::string& vertPath, const std::string& fragPath) {
    std::string vertCode, fragCode;
    std::ifstream vFile(vertPath), fFile(fragPath);

    if (vFile.is_open() && fFile.is_open()) {
        std::stringstream vStream, fStream;
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();
        vertCode = vStream.str();
        fragCode = fStream.str();
        return loadFromStrings(vertCode, fragCode);
    }

    std::cerr << "Failed to open shader files: " << vertPath << " / " << fragPath << std::endl;
    return false;
}

bool Shader::loadFromStrings(const std::string& vertSrc, const std::string& fragSrc) {
    GLuint vs = compile(GL_VERTEX_SHADER, vertSrc.c_str());
    if (vs == 0) return false;

    GLuint fs = compile(GL_FRAGMENT_SHADER, fragSrc.c_str());
    if (fs == 0) {
        glDeleteShader(vs);
        return false;
    }

    if (id != 0) glDeleteProgram(id);
    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader link error: " << infoLog << std::endl;
        glDeleteShader(vs);
        glDeleteShader(fs);
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}

void Shader::use() const {
    if (id != 0) {
        glUseProgram(id);
    }
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec4(const std::string& name, const glm::vec4& val) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform4fv(loc, 1, glm::value_ptr(val));
}

void Shader::setVec3(const std::string& name, const glm::vec3& val) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform3fv(loc, 1, glm::value_ptr(val));
}

void Shader::setVec2(const std::string& name, const glm::vec2& val) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform2fv(loc, 1, glm::value_ptr(val));
}

void Shader::setFloat(const std::string& name, float val) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform1f(loc, val);
}

void Shader::setInt(const std::string& name, int val) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform1i(loc, val);
}

void Shader::setBool(const std::string& name, bool val) const {
    GLint loc = glGetUniformLocation(id, name.c_str());
    if (loc != -1) glUniform1i(loc, val ? 1 : 0);
}
