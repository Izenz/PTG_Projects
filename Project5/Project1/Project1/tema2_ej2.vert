#version 150

uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uColorRotationMatrix;
uniform float uRadius;

in vec3 aVertex;
in vec2 aTexCoord;
in vec3 aNormal;

out vec4 vColor;
out vec2 vTexCoor;

void main()
{
	
	vec4 x = vec4(aVertex, 1.0);
	vec4 y = vec4(normalize(aVertex)*3, 1.0);

	float aux = 0.5 + 0.5 * sin(uRadius);

	gl_Position = uModelViewProjectionMatrix * mix(x, y, aux);
	vColor = abs(uColorRotationMatrix * vec4(aNormal,1));
	vTexCoor = aTexCoord;
}