#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec3 textColor;

void main()
{
    vec4 tex=texture(ourTexture, TexCoord);
    FragColor = mix(vec4(0.0),vec4(textColor.rgb,1.0),tex.r);
}