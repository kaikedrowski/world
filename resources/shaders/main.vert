#version 410 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 aPosV;
//out vec3 Normal;
out vec2 TexCoords;
//out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat4 lightSpaceMatrix;

void main()
{
    aPosV=aPos;
    //Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    //FragPosLightSpace=lightSpaceMatrix*vec4(FragPos,1.0);
    gl_Position = vec4(aPos, 1.0);
}