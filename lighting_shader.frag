#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
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

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcBumpedNormal(vec3 normal,vec3 tangent,vec3 bitangent);
void main()
{
    vec3 norm;
    //if(bump && (Tangent!=vec3(0.0f,0.0f,0.0f))&&(Bitangent!=vec3(0.0f,0.0f,0.0f))){
    if(bump){
        norm = CalcBumpedNormal(Normal,Tangent,Bitangent);
    }
    else{
        norm = normalize(Normal);
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result=CalcDirLight(dirLight, norm, viewDir);
    for(int i=0;i<POINTLIGHTS;i++){
     result += CalcPointLight(pointLight[i],norm,FragPos,viewDir);
    }
    result += CalcSpotLight(spotLight,norm,FragPos,viewDir);
    color = vec4(result,1.0f);
}
// Calculates the color when using a directional light.
#define SHININESS 128.0f
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material1.shininess);
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(material1.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material1.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material1.specular, TexCoords));
    //vec3 specular = light.specular * spec * vec3(0.0f);
    return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
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
    vec3 ambient  = light.ambient  * vec3(texture(material1.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material1.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material1.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
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
    vec3 ambient = light.ambient * vec3(texture(material1.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material1.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material1.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
vec3 CalcBumpedNormal(vec3 _normal,vec3 _tangent,vec3 _bitangent)
{
    vec3 normal = normalize(_normal);
    vec3 tangent = normalize(_tangent);
    tangent = normalize(tangent - dot(tangent, Normal) * Normal);
    vec3 bitangent = cross(tangent, normal);
    vec3 BumpMapNormal = texture(material1.normal, TexCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(tangent, bitangent, normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return -NewNormal;//why??, light looks inverted if I dont do this
}
