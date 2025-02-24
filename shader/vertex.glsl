#version 450 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;

// Values that stay constant for the whole mesh.
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec4 lightPos;
uniform float fTime;

//Output variables
out vec3 fNormal;
out vec3 fPosition;
out vec3 fLight;
out vec2 UV;
out vec3 vertexPos;
out vec3 vertexNormal;
out float iTime;

void main(){
	mat4 MV = V * M;
	mat4 MVP = P * V * M;
	vec3 vertexPos = vertexPosition_modelspace;
	vec3 vertexNormal = vertexNormal_modelspace;
	//Transform position and normal to camera space fragment shader for lighting
	vec4 positionHom = MV * vec4(vertexPosition_modelspace,1.0);
	fPosition = positionHom.xyz;
	mat4 normalMatrix = transpose(inverse(MV));
	vec4 normalHom = normalMatrix * vec4(vertexNormal_modelspace,1.0);
	fNormal = normalHom.xyz;
	vec4 lightHom = normalMatrix * lightPos;
	fLight = lightHom.xyz;

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// UV of the vertex. No special space for this one.
	UV = vertexUV;
     	iTime = fTime;
}