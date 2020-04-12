#version 150  

in vec3 aPosition; // Posición del vértice en el S.R. local al objeto
in vec3 aNormal; // Normal del vértice en el S.R. local al objeto
in vec3 aTangent;
in vec2 aTexCoord;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

out vec3 vLightDir;
out vec3 vViewDir;
out vec2 vST;

struct LightInfo {
	vec4 lightPos; // Posición de la luz (S.R. de la vista)
	vec3 intensity;
};
uniform LightInfo uLight;

void main()
{

	vec3 n = normalize(uNormalMatrix * aNormal);
	vec3 t = normalize(uNormalMatrix * aTangent);
	vec3 b = cross(n, t);
	mat3 TBN = transpose(mat3(t,b,n));

	vec3 eyeVertex = (uModelViewMatrix * vec4(aPosition,1.0)).xyz;
	vec3 eyeViewDir = vec3(0., 0., 0) - eyeVertex;
	vec3 eyeLightDir = uLight.lightPos.xyz - eyeVertex;

	vLightDir = TBN * eyeLightDir;
	vViewDir = TBN * eyeViewDir;

	vST = aTexCoord;
	gl_Position = uModelViewProjMatrix * vec4(aPosition, 1.0);
}