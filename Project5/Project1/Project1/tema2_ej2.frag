#version 150

uniform float uUmbral;

in vec4 vColor;
in vec2 vTexCoor;
out vec4 fFragColor;

void main()
{
	float value = 0.5 + 0.5 * sin(uUmbral);
	float s_aux = vTexCoor.s * 10.0f;
	float t_aux = vTexCoor.t * 10.0f;

	if(fract(s_aux) > value && fract(t_aux) > value)
		discard;

	fFragColor = vColor;
}
