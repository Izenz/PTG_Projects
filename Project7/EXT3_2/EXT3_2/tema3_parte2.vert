#version 150  

in vec3 aPosition; // Posici�n del v�rtice en el S.R. local al objeto
in vec3 aNormal; // Normal del v�rtice en el S.R. local al objeto
in vec2 aTexCoord;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

struct LightInfo {
	vec4 lightPos; // Posici�n de la luz (S.R. de la vista)
	vec3 intensity;
	vec3 k;
	vec3 dir; // Direcci�n del foco (S.R. de la vista)
	float cutOffInt; // �ngulo interior (l�mite zona iluminada)
	float cutOffExt; // �ngulo exterior (l�mite zona penumbra)
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
// Par�metros:	pos - posici�n del v�rtice en el S.R. de la vista
//				n - normal del v�rtice en el S.R. de la vista
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

void main()
{
	vec3 ecPos = vec3(uModelViewMatrix * vec4(aPosition, 1.0)); // Posici�n en el S.R. vista
	vec3 ecNorm = normalize(uNormalMatrix * aNormal); // Normal en el S.R. vista

}