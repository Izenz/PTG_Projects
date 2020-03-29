#version 150  

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uCameraPos;
uniform int uDrawSky;
uniform int uApplyChromaticDispersion;
uniform int uApplyFresnel;

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vTexCoord;

out float fresnel;
out vec3 vRefract;
out vec3 vReflect;
out float skyBox;

// Method prototypes & const variables

void PaintSkyBox(vec3);
void applyMetalMaterial(vec3, vec3);
void applyCrystalMaterial(vec3, vec3);

const float crystalRefractionRatio = 1.521;

void main()
{
	vRefract = vec3(0.0, 0.0, 0.0);
	vReflect = vec3(0.0, 0.0, 0.0);
	skyBox = 0.0;
	
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
	skyBox = 1.0;
}

void applyMetalMaterial (vec3 ecView, vec3 ecNormal) {
	vReflect = reflect(ecView, ecNormal);
	fresnel = 1;
}

void applyCrystalMaterial (vec3 ecView, vec3 ecNormal) {
	
	fresnel = 0;
	
	if(uApplyFresnel == 1)
	{
		float n1 = 1.0, n2 = 1.5;
		float f = pow((1.0 - n1/n2), 2.0) / pow((1.0 + n1/n2), 2.0);
		fresnel = f + (1.0 - f)*pow((1.0 - dot(ecNormal, ecView)), 5.0);
		vReflect = reflect(ecView, ecNormal);
		fresnel = fresnel * 0.01;
	}

	if(uApplyChromaticDispersion == 1)
	{
		vec3 R = refract(ecView, ecNormal, 1.506 * 2);
		vec3 G = refract(ecView, ecNormal, 1.520 * 2);
		vec3 B = refract(ecView, ecNormal, 1.535 * 2);
		vRefract = vec3(R.r, G.g, B.b);
	}
	else
		vRefract = refract(ecView, ecNormal, crystalRefractionRatio * 2);


	
	

}