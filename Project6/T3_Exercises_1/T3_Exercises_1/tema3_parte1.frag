#version 150 

uniform int uDrawSky;
uniform samplerCube uTexMap;

in vec3 vTexCoord;

out vec4 fFragColor;

void main()
{
	vec4 texColor = texture( uTexMap, vTexCoord );
	fFragColor = texColor;
}
