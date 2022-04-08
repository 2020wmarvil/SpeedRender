#ifndef MESH_H
#define MESH_H

#include "Core.h"
#include "Texture.h"
#include "Shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
    public:
        // mesh data
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture>      textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void Draw(Shader &shader);
    private:
        //  render data
        unsigned int VAO, VBO, EBO;

        void SetupMesh();
};  

#endif
