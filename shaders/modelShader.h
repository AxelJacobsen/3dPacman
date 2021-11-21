#ifndef __SQUARE_H_
#define __SQUARE_H_

#include <string>

static const std::string VertexShaderSrc = R"(
#version 430 core

uniform mat4 view;
uniform mat4 projection;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_normals;
layout(location = 2) in vec2 a_texture;

//We specify our uniforms. We do not need to specify locations manually, but it helps with knowing what is bound where.
layout(location=0) uniform mat4 u_TransformationMat = mat4(1);
layout(location=3) uniform mat4 u_LightSpaceMat     = mat4(1);

out vec4 vertexPositions;
out vec3 normals;
out vec4 FragPosLightSpace;
out vec2 modTexture;

void main()
{
vertexPositions = vec4(a_Position, 1.0);

FragPosLightSpace = u_LightSpaceMat *  u_TransformationMat * vertexPositions;

modTexture = a_texture;


mat3 normalmatrix = transpose(inverse(mat3(view * u_TransformationMat)));

normals = normalize(normalmatrix * normalize(a_normals));


gl_Position = projection * view * u_TransformationMat * vec4(a_Position, 1.0);
}
)";

static const std::string directionalLightFragmentShaderSrc = R"(
#version 430 core

in vec4 vertexPositions;
in vec3 normals;
in vec4 FragPosLightSpace;
in vec2 modTexture;
uniform mat4 view = mat4(1);

out vec4 color;

uniform mat4 u_TransformationMat = mat4(1);

//We need the shadowmap texture to make sure we can utilize the values stored in it to compute lighting
uniform sampler2D u_ShadowMap;

uniform vec4 u_Color;
uniform vec3 u_LightColor;
uniform vec3 u_LightDirection;
uniform float u_Specularity;

uniform sampler2D u_modelTexture;

//A new function for computing how shadowed a fragment is
float ShadowCalculation(
    in vec4 FragPosLightSpace,
    in vec3 normals
)
{
    vec3 projCoords = FragPosLightSpace.xyz;
    
    projCoords = (projCoords * 0.5) +0.5;

    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    
    float bias = max(0.05 * (1.0 - dot(normals, u_LightDirection)), 0.005);  

    float shadow;
    if(projCoords.z - bias > closestDepth)
    {
        shadow = 1.0;
    }
    else
    {
        shadow = 0.0;
    }
    
    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }   
    
    return shadow;
}

vec3 DirectionalLight(
    in vec3 color,
    in vec3 direction,
    in float shadow
)
{

    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * color;
    
    vec3 dir_to_light = normalize(-direction);
    vec3 diffuse = color * max(0.0, dot(normals, dir_to_light));                        

    vec3 viewDirection = normalize(vec3(inverse(view) * vec4(0,0,0,1) - u_TransformationMat * vertexPositions));
    
    vec3 reflectionDirection = reflect(dir_to_light,normals);                                                        
    
    float specular_power = pow(max(0.0,dot(viewDirection,reflectionDirection)),32);                                  
    vec3 specular = u_Specularity * specular_power * color;                                                          

    return ambient +(1.0-shadow) * (diffuse + specular);
}

void main()
{
float shadow = ShadowCalculation(FragPosLightSpace,normals);

vec3 light = DirectionalLight(u_LightColor,u_LightDirection,shadow);

vec4 textColorMod = texture(u_modelTexture, modTexture);

color = u_Color * textColorMod * vec4(light, 1.0);
}
)";

#endif // __SQUARE_H_
