#version 450 core

// Ouput data
out vec3 color;

//Input data
in vec3 fNormal;
in vec3 fPosition;
in vec3 fLight;
// Interpolated values from the vertex shaders
in vec2 UV;
in float iTime;
in vec4 gl_FragCoord;

void main(){
     // ambient lighting (global illumination)
     vec3 ambient = vec3(1.0,1.0,1.0);

     // Normal
     vec3 normal = normalize(fNormal);
     vec3 lightColor = vec3(1.0, 1.0, 1.0); // color white
     vec3 lightSource = vec3(0.0, 1.0, 0.0);
     float diffuseStrength = max(0.0, dot(lightSource, normal));
     lightSource = vec3(0.5, 0.0, 0.0);
     diffuseStrength += max(0.0, dot(lightSource, normal));	
     vec3 diffuse = diffuseStrength * lightColor;

     // lighting = ambient + diffuse + specular
     vec3 lighting = ambient * 0.0 + diffuse;
     
     vec3 modelColor = vec3(0.75, 0.75, 0.75);
     color = modelColor * lighting;
}
