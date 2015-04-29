#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;

    vec3 v3ambient;
    vec3 v3diffuse;
    vec3 v3specular;

    float shininess;
};



in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform Material material1;

void main()
{
    /*vec3 result = vec3(texture(material1.diffuse,TexCoords));
    color = vec4(result,1.0f);*/
    color = vec4(1.0f);
}
