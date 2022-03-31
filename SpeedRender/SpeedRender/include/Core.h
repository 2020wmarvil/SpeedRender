#ifndef CORE_H
#define CORE_H

#include <glm/glm.hpp>

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

typedef glm::vec3 Color;

#endif
