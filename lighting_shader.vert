#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 FragPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec2 TexCoords;

out mat3 TBN;

uniform mat4 model;
/*uniform mat4 view;
uniform mat4 projection;*/
uniform mat4 projXview;

//uniform sampler2D material1_normal;

void main()
{
    vec4 modelXposition=model * vec4(position, 1.0f);
    gl_Position = projXview * modelXposition;
    FragPos = modelXposition.xyz;
    TexCoords = texCoords;


    Normal = (model * vec4(normal, 0.0)).xyz;
    Normal = mat3(transpose(inverse(model))) * Normal;
    Tangent = (model * vec4(tangent, 0.0)).xyz;
    Bitangent = (model * vec4(bitangent, 0.0)).xyz;

}
