#version 150  

in vec3 aPosition; // Posición del vértice en el S.R. local al objeto
in vec3 aNormal; // Normal del vértice en el S.R. local al objeto
in vec2 aTexCoord;


uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;
uniform int uShading;
uniform int uLightType;

flat out int SHADING_TYPE;
flat out int LIGHT_TYPE;
out vec3 vColor;
out vec3 vECPos;
out vec3 vECNorm;
flat out vec3 flat_vColor;

struct LightInfo {
	vec4 lightPos; // Posición de la luz (S.R. de la vista)
	vec3 intensity;
	vec3 k;
	vec3 dir; // Dirección del foco (S.R. de la vista)
	float cutOffInt; // Ángulo interior (límite zona iluminada)
	float cutOffExt; // Ángulo exterior (límite zona penumbra)
};
uniform LightInfo uLight;

struct MaterialInfo {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform MaterialInfo uMaterial;

 
// ADS para la luz puntual
// Parámetros:	pos - posición del vértice en el S.R. de la vista
//				n - normal del vértice en el S.R. de la vista
vec3 ads(in vec3 pos, in vec3 n)
{
	vec3 ldir = normalize( vec3(uLight.lightPos) - pos );
	vec3 vdir = normalize( vec3(-pos) );
	vec3 r = reflect(-ldir, n);
	vec3 color;
	color = uLight.intensity * (   uMaterial.ambient 
								 + uMaterial.diffuse * max(dot(ldir,n), 0.0) 
								 + uMaterial.specular * pow(max(dot(r,vdir),0.0), uMaterial.shininess)
							   );

	return color;
}

vec3 adsSpotLight(in vec3 pos, in vec3 n){
	vec3 ldir = vec3( uLight.lightPos - vec4(pos, 1.0));
	float d = length(ldir);
	ldir = normalize(ldir);
	float angle = acos(dot(-ldir, normalize(uLight.dir)));
	vec3 I = uLight.intensity / (uLight.k[0] + uLight.k[1]*d + uLight.k[2]*d*d);
	vec3 ambient = I * uMaterial.ambient;
	I *= 1 - smoothstep(radians(uLight.cutOffInt), radians(uLight.cutOffExt), angle);
	vec3 r = normalize(reflect(-ldir, n));	
	vec3 view = normalize(vec3(-pos));
	return ambient + I * (	uMaterial.diffuse * max(dot(ldir, n), 0.0) +
							uMaterial.specular * pow(max(dot(r,view),0), uMaterial.shininess));
}

void main()
{
	vec3 ecPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0)); // Posición en el S.R. vista
	vec3 ecNorm = normalize(uNormalMatrix * aNormal); // Normal en el S.R. vista
	
	if(uLightType == 1){	// POINT LIGHT
		flat_vColor = clamp(ads(ecPos, ecNorm), 0.0, 1.0);
		vColor = clamp(ads(ecPos, ecNorm), 0.0, 1.0);
		LIGHT_TYPE = 0;
	}		
	else{					// SPOT LIGHT
		flat_vColor = clamp(adsSpotLight(ecPos, ecNorm), 0.0, 1.0);
		vColor = clamp(adsSpotLight(ecPos, ecNorm), 0.0, 1.0);
		LIGHT_TYPE = 1;
	}
		
	
	switch(uShading){
		case 0:			// FLAT SHADING
			SHADING_TYPE = 0;
			break;
		case 1:			// SOFT SHADING
			SHADING_TYPE = 1;
			break;
		case 2:			// PHONG SHADING
			SHADING_TYPE = 2;
			vECPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0));
			vECNorm = normalize(uNormalMatrix * aNormal);
			break;
		case 3:
			SHADING_TYPE = 3;
			vECPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0));
			vECNorm = normalize(uNormalMatrix * aNormal);
			break;
	}

	gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
}