// tessellation evaluation shader
#version 410 core

layout (triangles,equal_spacing,ccw) in;

in vec3 aPosTesc [];

uniform sampler2D heightMap;  // the texture corresponding to our height map
uniform mat4 model;           // the model matrix
uniform mat4 view;            // the view matrix
uniform mat4 projection;      // the projection matrix

// save for fragment
//in vec3 tc_FragPos [];
//in vec3 tc_Normal [];
//in vec4 tc_FragPosLightSpace [];
smooth out vec3 FragPos;
smooth out vec3 Normal;
//out vec4 FragPosLightSpace;

// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];

// send to Fragment Shader for coloring
out vec2 TexCoords;

out float Height;

void main()
{
    // pass fragment shader args through
    //FragPos=tc_FragPos[0];
    //Normal=tc_Normal[0];
    //FragPosLightSpace=tc_FragPosLightSpace[0];

    // get barycentric coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    //test
    //FragPos = u*tc_FragPos[0] + v*tc_FragPos[1] + w*tc_FragPos[2];
    vec3 aPos = u*aPosTesc[0]+v*aPosTesc[1]+w*aPosTesc[2];
    FragPos = vec3(model * vec4(aPos, 1.0));
    //FragPosLightSpace = u*tc_FragPosLightSpace[0] + v*tc_FragPosLightSpace[1] + w*tc_FragPosLightSpace[2];

    // ----------------------------------------------------------------------
    // retrieve control point texture coordinates
    vec2 t1 = TextureCoord[0];
    vec2 t2 = TextureCoord[1];
    vec2 t3 = TextureCoord[2];

    // barycentric interpolation of texture coordinate across patch
    vec2 texCoord = u * t1 + v * t2 + w * t3;

    TexCoords=texCoord;

    // lookup texel at patch coordinate for height and scale + shift as desired
    float height = texture(heightMap, texCoord).y * 0.0013876;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;
    vec4 p3 = gl_in[2].gl_Position;

    vec4 position = (u * p1 + v * p2 + w * p3);

    position=vec4(normalize(position.xyz),position.w);

    vec4 normal;
    if(u!=0&&v!=0&&w!=0)
        normal=normalize(vec4(cross((p1-p2).xyz,(p1-p3).xyz),0.0));
    else
        normal=vec4(position.xyz,0.0);

    // set Normal for fragment shader
    Normal=mat3(transpose(inverse(model))) * normalize(position.xyz);

    // displace point along normal
    position += normal * height;

    Height = height;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = projection * view * model * position;
}