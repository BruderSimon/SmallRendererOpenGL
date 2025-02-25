#version 450 core

// Output data
out vec3 color;

// Input data
in vec3 fNormal;       // Interpolated normal from vertex shader
in vec3 fPosition;     // Interpolated position from vertex shader
in vec3 fLight;        // Interpolated light position from vertex shader
in vec2 UV;            // Interpolated UV coordinates
in float iTime;        // Interpolated time (unused in this example)
in vec4 gl_FragCoord;  // Fragment coordinates (unused in this example)

// Phong lighting parameters
uniform vec3 lightPos;  // Light position in world space
uniform vec3 viewPos;   // Camera position in world space
uniform float shininess; // Shininess factor for specular reflection

void main() {
    // Ambient lighting (global illumination)
    vec3 ambient = vec3(0.1, 0.1, 0.1); // Low-intensity ambient light

    // Normalize the interpolated normal
    vec3 normal = normalize(fNormal);

    // Light properties
    vec3 lightColor = vec3(1.0, 1.0, 1.0); // White light
    vec3 lightDir = normalize(lightPos - fPosition); // Direction from fragment to light

    // Diffuse lighting
    float diffuseStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - fPosition); // Direction from fragment to camera
    vec3 reflectDir = reflect(-lightDir, normal);  // Reflection of light direction
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * lightColor;

    // Combine all lighting components
    vec3 lighting = ambient + diffuse + specular;

    // Base color of the model
    vec3 modelColor = vec3(0.75, 0.75, 0.75); // Gray color

    // Final color
    color = modelColor * lighting;
}