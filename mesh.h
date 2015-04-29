#ifndef MESH
#define MESH

extern "C"{
#define GLEW_STATIC
#include <GL/glew.h> // Include glew to get all the required OpenGL headers
}
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct Vertex {
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;

    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};
enum TextureType{
  DIFFUSE,
  SPECULAR,
  NORMAL
};
string strTextureType(TextureType type){
    switch(type){
        case DIFFUSE:
            return "diffuse";
        case SPECULAR:
            return "specular";
        case NORMAL:
            return "normal";
    }
    return "error";
}
struct Texture {
    GLuint id;
//    string type;
    TextureType type;
    aiString path;
};
struct Textures{
  vector<Texture> diffuse;
  vector<Texture> specular;
  vector<Texture> normal;
};

class Mesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    Textures textures;

    float shininess;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    //should use a material struct, right now
    //if we have more than 1 material this fucks up
    /*  Functions  */
    // Constructor
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures,float _shininess=16.0f
            ,glm::vec3 _ambient=glm::vec3(1.0f)
            ,glm::vec3 _diffuse=glm::vec3(1.0f)
            ,glm::vec3 _specular=glm::vec3(1.0f))
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures2 = textures;
        this->shininess=_shininess;
        this->ambient=_ambient;
        this->diffuse=_diffuse;
        this->specular=_specular;

#if 0

        unsigned int indicesCount=this->indices.size();
        for (unsigned int i = 0 ; i < indicesCount ; i += 3) {
            Vertex& v0 = this->vertices[this->indices[i]];
            Vertex& v1 = this->vertices[this->indices[i+1]];
            Vertex& v2 = this->vertices[this->indices[i+2]];
            v0.Tangent=glm::vec3(0.0f);
            v1.Tangent=glm::vec3(0.0f);
            v2.Tangent=glm::vec3(0.0f);
            v0.Bitangent=glm::vec3(0.0f);
            v1.Bitangent=glm::vec3(0.0f);
            v2.Bitangent=glm::vec3(0.0f);

            glm::vec3 Edge1 = v1.Position - v0.Position;
            glm::vec3 Edge2 = v2.Position - v0.Position;

            float DeltaU1 = v1.TexCoords.x - v0.TexCoords.x;
            float DeltaV1 = v1.TexCoords.y - v0.TexCoords.y;
            float DeltaU2 = v2.TexCoords.x - v0.TexCoords.x;
            float DeltaV2 = v2.TexCoords.y - v0.TexCoords.y;

            float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

            glm::vec3 Tangent, Bitangent;

            Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
            Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
            Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

            Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
            Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
            Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);

            v0.Tangent += Tangent;
            v1.Tangent += Tangent;
            v2.Tangent += Tangent;

            v0.Bitangent += Bitangent;
            v1.Bitangent += Bitangent;
            v2.Bitangent += Bitangent;
        }

        for (unsigned int i = 0 ; i < vertices.size() ; i++) {
            vertices[i].Tangent=glm::normalize(vertices[i].Tangent);
            vertices[i].Bitangent=glm::normalize(vertices[i].Bitangent);
        }

#endif

        // Now that we have all the required data, set the vertex buffers and its attribute pointers.
        this->setupMesh();
    }
    Mesh(vector<Vertex> vertices, vector<GLuint> indices,Textures textures,float _shininess=16.0f
            ,glm::vec3 _ambient=glm::vec3(1.0f)
            ,glm::vec3 _diffuse=glm::vec3(1.0f)
            ,glm::vec3 _specular=glm::vec3(1.0f))
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->shininess=_shininess;
        this->ambient=_ambient;
        this->diffuse=_diffuse;
        this->specular=_specular;
#if 0

        unsigned int indicesCount=this->indices.size();
        for (unsigned int i = 0 ; i < indicesCount ; i += 3) {
            Vertex& v0 = this->vertices[this->indices[i]];
            Vertex& v1 = this->vertices[this->indices[i+1]];
            Vertex& v2 = this->vertices[this->indices[i+2]];
            v0.Tangent=glm::vec3(0.0f);
            v1.Tangent=glm::vec3(0.0f);
            v2.Tangent=glm::vec3(0.0f);
            v0.Bitangent=glm::vec3(0.0f);
            v1.Bitangent=glm::vec3(0.0f);
            v2.Bitangent=glm::vec3(0.0f);

            glm::vec3 Edge1 = v1.Position - v0.Position;
            glm::vec3 Edge2 = v2.Position - v0.Position;

            float DeltaU1 = v1.TexCoords.x - v0.TexCoords.x;
            float DeltaV1 = v1.TexCoords.y - v0.TexCoords.y;
            float DeltaU2 = v2.TexCoords.x - v0.TexCoords.x;
            float DeltaV2 = v2.TexCoords.y - v0.TexCoords.y;

            float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

            glm::vec3 Tangent, Bitangent;

            Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
            Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
            Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

            Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
            Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
            Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);

            v0.Tangent += Tangent;
            v1.Tangent += Tangent;
            v2.Tangent += Tangent;

            v0.Bitangent += Bitangent;
            v1.Bitangent += Bitangent;
            v2.Bitangent += Bitangent;
        }

        for (unsigned int i = 0 ; i < vertices.size() ; i++) {
            vertices[i].Tangent=glm::normalize(vertices[i].Tangent);
            vertices[i].Bitangent=glm::normalize(vertices[i].Bitangent);
        }
#endif


        // Now that we have all the required data, set the vertex buffers and its attribute pointers.
        this->setupMesh();
    }

    // Render the mesh
    void Draw(Shader shader)
    {
        GLuint textureCount=this->textures.diffuse.size();
        GLuint i=0;
        GLuint lastText=0;
        GLuint num_diffuse=0,num_specular=0,num_normal=0;

        static char str_diffuse[]="materialN.diffuse";
        for(char matnumber='1';i<textureCount;i++){
            glActiveTexture(GL_TEXTURE0 + lastText + i);

            str_diffuse[8]=matnumber;
            glUniform1f(glGetUniformLocation(shader.Program, str_diffuse), i);
            matnumber++;
            glBindTexture(GL_TEXTURE_2D, textures.diffuse[i].id);
            num_diffuse++;
        }
        if(!i){
            //no diffuse textures found
        }

        lastText+=i;
        static char str_specular[]="materialN.specular";
        textureCount=this->textures.specular.size();
        i=0;
        for(char matnumber='1';i<textureCount;i++){
            glActiveTexture(GL_TEXTURE0 +lastText+i);

            str_specular[8]=matnumber;
            glUniform1f(glGetUniformLocation(shader.Program, str_specular), i);
            matnumber++;
            glBindTexture(GL_TEXTURE_2D, textures.specular[i].id);
            num_specular++;
        }
        if(!i){
            //no specular found
        }

        lastText+=i;

        static char str_normal[]="materialN.normal";
        textureCount=this->textures.normal.size();
        i=0;
        for(char matnumber='1';i<textureCount;i++){
            glActiveTexture(GL_TEXTURE0 + lastText + i);

            str_normal[8]=matnumber;
            glUniform1f(glGetUniformLocation(shader.Program, str_normal), i);
            matnumber++;
            glBindTexture(GL_TEXTURE_2D, textures.normal[i].id);
            num_normal++;
        }
        if(!i){
            //no normal found
        }
        lastText+=i;

        //this is wrong!!!, should do it for each material
        static char str_shininess[]="material1.shininess";
        static char str_v3ambient[]="material1.v3ambient";
        static char str_v3diffuse[]="material1.v3diffuse";
        static char str_v3specular[]="material1.v3specular";
        static char str_has_diffuse[]="material1.has_diffuse";
        static char str_has_specular[]="material1.has_specular";
        static char str_has_normal[]="material1.has_normal";
        char num_mat='0'+max(max(num_diffuse,num_specular),num_normal);
        for(char it='1';it<=num_mat;it++){
            str_shininess[8]=it;
            str_v3ambient[8]=it;
            str_v3diffuse[8]=it;
            str_v3specular[8]=it;
            str_has_diffuse[8]=it;
            str_has_specular[8]=it;
            str_has_normal[8]=it;
            glUniform1f(glGetUniformLocation(shader.Program, str_shininess), shininess);
            glUniform3f(glGetUniformLocation(shader.Program, str_v3ambient), ambient.x,ambient.y,ambient.z);
            glUniform3f(glGetUniformLocation(shader.Program, str_v3diffuse), diffuse.x,diffuse.y,diffuse.z);
            glUniform3f(glGetUniformLocation(shader.Program, str_v3specular), specular.x,specular.y,specular.z);
        }

        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured.
        for ( i = 0; i < lastText; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
#if 0
        //should probably do something if it does not find a specular or normal map!!
        // Bind appropriate textures
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        GLuint normalNr = 1;
        GLuint textureCount=this->textures.size();
        static char str_diffuse[]="materialN.diffuse";
        static char str_specular[]="materialN.specular";
        static char str_normal[]="materialN.normal";
        static char str_shininess[]="materialN.shininess";
        static char str_v3ambient[]="materialN.v3ambient";
        static char str_v3diffuse[]="materialN.v3diffuse";
        static char str_v3specular[]="materialN.v3specular";
        static char str_has_diffuse[]="materialN.has_diffuse";
        static char str_has_specular[]="materialN.has_specular";
        static char str_has_normal[]="materialN.has_normal";

        for(GLuint i = 0; i < textureCount; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding

            // Retrieve texture number (the N in diffuse_textureN)
//            stringstream ss;
//            string number;
            GLuint uinumber=1;
            Texture& text=this->textures[i];
            TextureType type = text.type;
            char* str_type=NULL;
            switch(type){
                case DIFFUSE:
                    uinumber = (diffuseNr);
                    ++diffuseNr;
                    str_type=str_diffuse;
                    has_diffuse=true;
                    break;
                case SPECULAR:
                    uinumber = (specularNr);
                    ++specularNr;
                    str_type=str_specular;
                    has_specular=true;
                    break;
                case NORMAL:
                    uinumber = (normalNr);
                    ++normalNr;
                    str_type=str_normal;
                    has_normal=true;
                    break;
                default:
                    std::cerr<<"unknown texture type, enum = "<<type<<std::endl;
                    assert(0);
            }
            //            number = ss.str();
            //            std::cout<<"material"<<uinumber<<'.'<<strTextureType(name)<<std::endl;
            //             Now set the sampler to the correct texture unit
            //            glUniform1f(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
            //            string str="material"s+(char)('0'+uinumber)+'.';

            //string str="materialN.";
            char material_number = ('0'+uinumber);
            str_type[8]=material_number;
            str_shininess[8]=material_number;
            str_v3ambient[8]=material_number;
            str_v3diffuse[8]=material_number;
            str_v3specular[8]=material_number;
            str_has_diffuse[8]=material_number;
            str_has_specular[8]=material_number;
            str_has_normal[8]=material_number;


            glUniform1f(glGetUniformLocation(shader.Program, str_type), i);
            glUniform1f(glGetUniformLocation(shader.Program, str_shininess), shininess);
            glUniform1i(glGetUniformLocation(shader.Program,str_has_diffuse),has_diffuse);
            glUniform1i(glGetUniformLocation(shader.Program,str_has_specular),has_specular);
            glUniform1i(glGetUniformLocation(shader.Program,str_has_normal),has_normal);
            glUniform3f(glGetUniformLocation(shader.Program, str_v3ambient), ambient.x,ambient.y,ambient.z);
            glUniform3f(glGetUniformLocation(shader.Program, str_v3diffuse), diffuse.x,diffuse.y,diffuse.z);
            glUniform3f(glGetUniformLocation(shader.Program, str_v3specular), specular.x,specular.y,specular.z);
            // And finally bind the texture
            glBindTexture(GL_TEXTURE_2D, text.id);

        }

        // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
        //glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);

        // Draw mesh
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured.
        for (GLuint i = 0; i < this->textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
#endif
    }

private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;

    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setupMesh()
    {
        // Create buffers/arrays
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);

        glBindVertexArray(this->VAO);
        // Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        // Vertex Normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};
#endif

