#version 150  

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;
uniform int uDrawSky;
uniform int uApplyChromaticDispersion;

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vTexCoord;

// Method prototypes & const variables

void PaintSkyBox(vec3);
void applyMetalMaterial(vec3, vec3);
void applyCrystalMaterial(vec3, vec3);

const float crystalRefractionRatio = 1.521;

void main()
{
	vec4 pos = vec4(aPosition, 1);
	vec3 ecNormal = normalize(uNormalMatrix * aNormal);
	vec3 ecView = (uModelMatrix * pos).xyz - uCameraPos;

	if(uDrawSky == 0)	// We're drawing skybox
		PaintSkyBox(ecView);
	else {				// We're drawing objects
		if (uDrawSky == 2)
			applyCrystalMaterial(ecView, ecNormal);
		else
			applyMetalMaterial(ecView, ecNormal);
	}

	gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
}


void PaintSkyBox (vec3 ecView) {
	vTexCoord = ecView;
}

void applyMetalMaterial (vec3 ecView, vec3 ecNormal) {
	vTexCoord = reflect(ecView, ecNormal);
}

void applyCrystalMaterial (vec3 ecView, vec3 ecNormal) {
	if(uApplyChromaticDispersion == 1)
		vTexCoord = vec3(	refract(ecView, ecNormal, 1.506),
							refract(ecView, ecNormal, 1.520),
							refract(ecView, ecNormal, 1.535));
	else
		vTexCoord = refract(ecView, ecNormal, crystalRefractionRatio * 2);
}