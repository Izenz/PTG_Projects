#version 150 

uniform int uDrawSky;
uniform samplerCube uTexMap;

in vec3 vTexCoord;
in vec3 vReflect;
in vec3 vRefract;
in float fresnel;
in float skyBox;

out vec4 fFragColor;


void main()
{
	vec4 texColor;
	if(skyBox == 1)
		texColor = texture( uTexMap, vTexCoord );
	else
	{
		vec3 mixeado = mix(vRefract, vReflect, fresnel);
		texColor = texture(uTexMap, mixeado);
	}

	fFragColor = texColor;
}
