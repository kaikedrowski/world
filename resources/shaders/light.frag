#version 330 core
out vec4 FragColor;

in vec3 FragPos;

uniform vec3 circleCenter;
uniform float circleRadius;

void main()
{
    float distanceToCenter=distance(FragPos.xyz,circleCenter);
    float alpha=smoothstep(circleRadius,circleRadius/2.0,distanceToCenter);
    FragColor=vec4(alpha);
}