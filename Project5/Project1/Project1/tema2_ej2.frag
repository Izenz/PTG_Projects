#version 150

uniform float uUmbral;
uniform sampler2D uStoneTexture;
uniform sampler2D uMossTexture;
uniform int uTexSelection;

in vec4 vColor;
in vec2 vTexCoor;
out vec4 fFragColor;

void main()
{
	float value = 0.5 + 0.5 * sin(uUmbral);
	float s_aux = vTexCoor.s * 10.0f;
	float t_aux = vTexCoor.t * 10.0f;

	vec4 stoneTexColor = texture(uStoneTexture, vTexCoor);
	vec4 mossTexColor = texture(uMossTexture, vTexCoor);

	if(fract(s_aux) > value && fract(t_aux) > value)
		discard;

	switch(uTexSelection){
		case 0:
			fFragColor = vColor;
			break;
		case 1:
			fFragColor = stoneTexColor * vColor;
			break;
		case 2:
			fFragColor = mix(stoneTexColor, mossTexColor, mossTexColor.a) *  vColor;
			break;
	}
}
