#ifndef CORE_H
#define CORE_H

#include <glm/glm.hpp>

#include "Texture.h"

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

typedef glm::vec3 Color;

struct LightProfile {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct Material {
    Texture diffuseMap;
    Texture specularMap;
    float shininess;
};

#endif
