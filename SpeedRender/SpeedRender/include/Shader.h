#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
  
class Shader {
public:
    // the program ID
    unsigned int ID;
  
    // constructor reads and builds the shader
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void Use() const;
    // utility uniform functions
    void SetBool(const std::string &name, bool value) const;  
    void SetInt(const std::string &name, int value) const;   
    void SetFloat(const std::string &name, float value) const;
    void SetMat4(const std::string &name, const glm::mat4 &value) const;
};
  
#endif
