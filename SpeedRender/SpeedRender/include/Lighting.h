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

struct LightProfile {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class DirectionalLight {
public:
    DirectionalLight(const glm::vec3 &direction, const Color &color, const LightProfile &lightProfile);

    glm::vec3 direction;
    Color color;
	LightProfile lightProfile;
};
  
class PointLight {
public:
    PointLight(const glm::vec3 &position, const glm::vec3 &scale, const Color &color, const LightProfile &lightProfile);
    void Draw(const glm::mat4 &view, const glm::mat4 &projection) const;

    glm::vec3 position;
    glm::vec3 scale;
    Color color;
	LightProfile lightProfile;
private:
    unsigned int VAO, VBO;
    Shader shader;
};

class SpotLight {
public:
    SpotLight(const glm::vec3 &position, const glm::vec3 &scale, const Color &color, const LightProfile &lightProfile);
    void Draw(const glm::mat4 &view, const glm::mat4 &projection) const;

    glm::vec3 position;
    glm::vec3 scale;
    Color color;
	LightProfile lightProfile;
private:
    unsigned int VAO, VBO;
    Shader shader;
};
  
#endif
