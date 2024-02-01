#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1);
    FragPos = vec3(M * vec4(vertexPosition, 1.0));
    Normal = mat3(transpose(inverse(M))) * vertexNormal;
    UV = vertexUV;
}