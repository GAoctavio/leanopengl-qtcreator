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

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;

in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

in vec2 TexCoords;
//in mat3 TBN;

out vec4 color;


uniform vec3 viewPos;
uniform DirLight dirLight;
#define POINTLIGHTS 2
uniform PointLight pointLight[POINTLIGHTS];//should make an array
uniform SpotLight spotLight;//should also make an array
uniform Material material1;

uniform bool bump;

vec3 CalcDirLight(Material material,DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Material material,PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Material material,SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcBumpedNormal(Material material,vec3 normal,vec3 tangent,vec3 bitangent);
void main()
{
    vec3 norm;
    if(bump){
        norm = CalcBumpedNormal(material1,Normal,Tangent,Bitangent);
    }
    else{
        norm = normalize(Normal);
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result=CalcDirLight(material1,dirLight, norm, viewDir);
    for(int i=0;i<POINTLIGHTS;i++){
     result += CalcPointLight(material1,pointLight[i],norm,FragPos,viewDir);
    }
    result += CalcSpotLight(material1,spotLight,norm,FragPos,viewDir);
    color = vec4(result,1.0f);
}
// Calculates the color when using a directional light.
#define SHININESS 128.0f
vec3 CalcDirLight(Material material,DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material1.shininess);
    // Combine results
    vec3 result=vec3(0.0f);
    result += light.ambient * vec3(texture(material.diffuse, TexCoords))*material.v3ambient;
    result += light.diffuse * diff * (vec3(texture(material.diffuse, TexCoords)))*material.v3diffuse;
    result += light.specular * spec * (vec3(texture(material.specular, TexCoords)))*material.v3specular;
    return (result);
}
vec3 CalcPointLight(Material material,PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material1.shininess);
    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance +
                             light.quadratic * (distance * distance));
    // Combine results
    vec3 result=vec3(0.0f);
    result += light.ambient * vec3(texture(material.diffuse, TexCoords))*material.v3ambient;
    result += light.diffuse * diff * (vec3(texture(material.diffuse, TexCoords)))*material.v3diffuse;
    result += light.specular * spec * (vec3(texture(material.specular, TexCoords)))*material.v3specular;
    result *= attenuation;
    return (result);
}


vec3 CalcSpotLight(Material material,SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material1.shininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // Combine results
    vec3 result=vec3(0.0f);
    result += light.ambient * vec3(texture(material.diffuse, TexCoords))*material.v3ambient;
    result += light.diffuse * diff * (vec3(texture(material.diffuse, TexCoords)))*material.v3diffuse;
    result += light.specular * spec * (vec3(texture(material.specular, TexCoords)))*material.v3specular;
    result *= attenuation * intensity;

    return (result);
}
vec3 CalcBumpedNormal(Material material,vec3 _normal,vec3 _tangent,vec3 _bitangent)
{
    vec3 normal = normalize(_normal);
    /*
    if(!material.has_normal) return normal;*/

    vec3 tangent = normalize(_tangent);
    tangent = normalize(tangent - dot(tangent, Normal) * Normal);
    vec3 bitangent = cross(tangent, normal);
    vec3 BumpMapNormal = texture(material.normal, TexCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(tangent, bitangent, normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return -NewNormal;//why??, light looks inverted if I dont do this
    //return NewNormal;
}
