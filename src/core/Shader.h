#pragma once
// ====================================================================
// FILE:     src/core/Shader.h
// Loads, compiles and links shader programs and sets uniforms.
// ====================================================================

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    GLuint id = 0;

    Shader() = default;
    ~Shader();

    bool loadFromFiles(const std::string& vertPath, const std::string& fragPath);
    bool loadFromStrings(const std::string& vertSrc, const std::string& fragSrc);

    void use() const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec4(const std::string& name, const glm::vec4& val) const;
    void setVec3(const std::string& name, const glm::vec3& val) const;
    void setVec2(const std::string& name, const glm::vec2& val) const;
    void setFloat(const std::string& name, float val) const;
    void setInt(const std::string& name, int val) const;
    void setBool(const std::string& name, bool val) const;

private:
    GLuint compile(GLenum type, const char* src);
};
