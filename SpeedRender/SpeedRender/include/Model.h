#ifndef MODEL_H
#define MODEL_H

#include "Core.h"
#include "Shader.h"
#include "Mesh.h"

class Model {
    public:
        Model(char* path);
        void Draw(Shader &shader);	
    private:
        // model data
        std::vector<Mesh> meshes;
        std::string directory;

        //void LoadModel(std::string path);
        //void ProcessNode(aiNode *node, const aiScene *scene);
        //Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
        //std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif
