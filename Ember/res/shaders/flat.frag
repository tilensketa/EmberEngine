#version 460 core

out vec4 FragColor;

uniform vec3 uFlatColor;

void main()
{
    FragColor = vec4(uFlatColor, 1.0f);
}
