#include "Model.h"

Model::Model(char* path) {
    LoadModel(path);
}

void Model::Draw(Shader &shader) {
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}  