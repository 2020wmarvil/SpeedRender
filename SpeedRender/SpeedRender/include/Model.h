#ifndef MODEL_H
#define MODEL_H

#include "cy/cyTriMesh.h"

#include "Core.h"
#include "Shader.h"
#include "Mesh.h"

class Model {
    public:
        Model(std::string path);
        void Draw(Shader &shader);	
    private:
        // model data
        std::vector<Mesh> meshes;
        void LoadModel(std::string path);
};

#endif
