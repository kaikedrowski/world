// tessellation control shader
#version 410 core

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=3) out;

in vec3 aPosV [];

uniform mat4 model;
uniform mat4 view;

// save for fragment
in vec3 FragPos [];
//in vec3 Normal [];
//in vec4 FragPosLightSpace [];
out vec3 aPosTesc [];
//out vec3 tc_Normal [];
//out vec4 tc_FragPosLightSpace [];

// varying input from vertex shader
in vec2 TexCoords[];
// varying output to evaluation shader
out vec2 TextureCoord[];

void main()
{
    // pass fragment shader args through
    aPosTesc[gl_InvocationID]=aPosV[gl_InvocationID];
    //tc_Normal[gl_InvocationID]=Normal[gl_InvocationID];
    //tc_FragPosLightSpace[gl_InvocationID]=FragPosLightSpace[gl_InvocationID];

    // ----------------------------------------------------------------------
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoords[gl_InvocationID];

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {

        vec3 viewPos0 = ( view * model * gl_in[0].gl_Position).xyz;
        vec3 viewPos1 = ( view * model * gl_in[1].gl_Position).xyz;
        vec3 viewPos2 = ( view * model * gl_in[2].gl_Position).xyz;

        float distance = (length(viewPos0) + length(viewPos1) + length(viewPos2)) / (3.0 * 10.0);

        float tessellationLevel=1;
        if(distance<=0.03125){tessellationLevel=64;}
        else if(distance<=0.0625){tessellationLevel=32;}
        else if(distance<=0.125){tessellationLevel=16;}
        else if(distance<=0.25){tessellationLevel=8;}
        else if(distance<=0.5){tessellationLevel=4;}
        else if(distance<=1){tessellationLevel=2;}

        gl_TessLevelOuter[0] = tessellationLevel;
        gl_TessLevelOuter[1] = tessellationLevel;
        gl_TessLevelOuter[2] = tessellationLevel;
        gl_TessLevelOuter[3] = tessellationLevel;

        gl_TessLevelInner[0] = tessellationLevel;
        gl_TessLevelInner[1] = tessellationLevel;
    }
}