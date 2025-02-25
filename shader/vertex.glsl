#version 450 core

// Input vertex data
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;

// Uniforms
uniform mat4 M;          // Model matrix
uniform mat4 V;          // View matrix
uniform mat4 P;          // Projection matrix
uniform vec3 lightPos;   // Light position in world space
uniform float fTime;     // Time (optional)

// Output variables
out vec3 fNormal;        // Transformed normal
out vec3 fPosition;      // Transformed position
out vec3 fLight;         // Transformed light position
out vec2 UV;             // UV coordinates
out float iTime;         // Time (optional)

void main() {
    // Model-view matrix
    mat4 MV = V * M;

    // Model-view-projection matrix
    mat4 MVP = P * MV;

    // Transform position to camera space
    vec4 positionHom = MV * vec4(vertexPosition_modelspace, 1.0);
    fPosition = positionHom.xyz;

    // Transform normal to camera space
    mat3 normalMatrix = transpose(inverse(mat3(MV)));
    fNormal = normalize(normalMatrix * vertexNormal_modelspace);

    // Transform light position to camera space
    fLight = (V * vec4(lightPos, 1.0)).xyz;

    // Output position of the vertex, in clip space
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);

    // Pass UV coordinates to the fragment shader
    UV = vertexUV;

    // Pass time to the fragment shader
    iTime = fTime;
}