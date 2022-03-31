#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Core.h"
#include "Shader.h"
  
class Light {
public:
    Light(const glm::vec3 &position, const glm::vec3 &scale, const Color &color);
    void Draw(const glm::mat4 &view, const glm::mat4 &projection) const;
    void SetPosition(const glm::vec3& position);
    glm::vec3 GetPosition() { return position; }
    Color GetColor() { return color; }
private:
    unsigned int VAO, VBO;
    Shader shader;
    glm::vec3 position;
    glm::vec3 scale;
    Color color;
};
  
#endif
