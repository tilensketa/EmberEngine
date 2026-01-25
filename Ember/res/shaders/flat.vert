#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aTangent;

uniform mat4 uCamMatrix;
uniform mat4 uModel;

void main()
{
    vec3 fragPos = vec3(uModel * vec4(aPos, 1.0));
    gl_Position = uCamMatrix * vec4(fragPos, 1.0);
}
