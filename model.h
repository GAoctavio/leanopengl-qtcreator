#ifndef MODEL
#define MODEL
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
extern "C"{
#define GLEW_STATIC
#include <GL/glew.h> // Include glew to get all the required OpenGL headers
#include <SOIL/SOIL.h>
}
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "mesh.h"

GLint TextureFromFile(const char* path, string directory);

class Model
{
public:
    /*  Functions   */
    // Constructor, expects a filepath to a 3D model.
    Model(GLchar* path)
    {
        this->loadModel(path);
    }

    // Draws the model, and thus all its meshes
    void Draw(Shader shader)
    {
        GLuint meshesCount=this->meshes.size();
        for(GLuint i = 0; i < meshesCount; i++)
            this->meshes[i].Draw(shader);
    }

private:
    /*  Model Data  */
    vector<Mesh> meshes;
    string directory;

    vector<Texture> textures_loaded;
    /*  Functions   */
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string path)
    {
        // Read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* sceneaux = importer.ReadFile(path, aiProcess_Triangulate           |
                                                       aiProcess_FlipUVs               |
                                                       aiProcess_JoinIdenticalVertices |
                                                       aiProcess_GenSmoothNormals);

        const aiScene* scene = importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
        // Check for errors
        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // Retrieve the directory path of the filepath
        this->directory = path.substr(0, path.find_last_of('/'));

        // Process ASSIMP's root node recursively
        this->processNode(scene->mRootNode, scene);
    }

    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // Process each mesh located at the current node
        for(GLuint i = 0; i < node->mNumMeshes; i++)
        {
            // The node object only contains indices to index the actual objects in the scene.
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // Data to fill
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        float shininess;

        // Walk through each of the mesh's vertices
        for(GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // Positions
            //auto dummy=*mesh;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            if(mesh->mTangents){
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else{
                vertex.Tangent = glm::vec3(0.0f);
                vertex.Bitangent = glm::vec3(0.0f);
            }

            // Texture Coordinates
            if(mesh->mTextureCoords[0])  // Does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            vertices.push_back(vertex);
        }
        std::cout<<"numVertices "<<mesh->mNumVertices<<std::endl;
        std::cout<<"numFaces "<<mesh->mNumFaces<<std::endl;

        // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices vector
            for(GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // Process materials
        if(mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
            // Same applies to other texture as the following list summarizes:
            // Diffuse: texture_diffuseN
            // Specular: texture_specularN
            // Normal: texture_normalN
            // actually materialN.diffuse, materialN.specular,materialN.normal

            // 1. Diffuse maps
            vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. Specular maps
            vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            vector<Texture> normalMaps = this->loadMaterialTextures(material,aiTextureType_HEIGHT,NORMAL);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            /*aiString str;
            material->Get(AI_MATKEY_TEXTURE_NORMALS,&str);
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = aiTextureType_NORMALS;
            texture.path = str;
            textures.push_back(texture);
            this->textures_loaded.push_back(texture);*/

            //shininess
            material->Get(AI_MATKEY_SHININESS,shininess);
        }


        // Return a mesh object created from the extracted mesh data
        //shininess in this library is from 0.0f-1000.0f
        return Mesh(vertices, indices, textures,shininess*256.0/1000.0);
    }

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.

    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName)
    {
        vector<Texture> textures;
        GLuint texCount=mat->GetTextureCount(type);
        for(GLuint i = 0; i < texCount; i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            GLboolean skip = false;
            GLuint texturesLoadedCount=textures_loaded.size();
            for(GLuint j = 0; j < texturesLoadedCount; j++)
            {
                if(textures_loaded[j].path == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if(!skip)
            {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                this->textures_loaded.push_back(texture);  // Add to loaded textures
            }
        }
        return textures;
    }
};



GLint TextureFromFile(const char* path, string directory)
{
     //Generate texture ID and load texture data
    string filename = string(path);
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}
#endif // MODEL

