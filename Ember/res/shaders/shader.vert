#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aTangent;

uniform mat4 uCamMatrix;
uniform mat4 uModel;

out vec3 fragPos;
out vec2 texCoords;
out mat3 TBN;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));

    // Transform N and T consistently
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    // T = normalize(T - dot(T, N) * N);
    T = normalize(T - N * dot(T, N));
    vec3 B = cross(N, T);


    TBN = mat3(T, B, N);
    texCoords = aTex;

    fragPos = vec3(uModel * vec4(aPos, 1.0));
    gl_Position = uCamMatrix * vec4(fragPos, 1.0);
}
