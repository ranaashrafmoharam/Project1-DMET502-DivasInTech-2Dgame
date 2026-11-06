#pragma once
// ====================================================================
// FILE:     src/core/Shader.h
// OWNER:    Member 1
// TASKS:    M1-01
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Loads, compiles and links shader programs from the
//   assets/shaders folder and lets other code set uniforms.
// ====================================================================

#if __has_include(<glad/glad.h>)
#include <glad/glad.h>
#elif __has_include(<glad/gl.h>)
#include <glad/gl.h>
#endif

#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    unsigned int ID;

    Shader();
    ~Shader();

    bool load(const std::string& vertPath, const std::string& fragPath);
    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    unsigned int compileShader(unsigned int type, const std::string& source, const std::string& stageName);
    std::string readFile(const std::string& filePath);
};
