#version 150 

in vec3 vLightDir;
in vec3 vViewDir;
in vec2 vST;

uniform sampler2D uNormalMap;
uniform sampler2D uColorMap;
uniform sampler2D uGlossMap;
uniform sampler2D uHeightMap;
uniform int uApplyNM;
uniform int uApplyCM;
uniform int uApplyGloss;
uniform int uApplyHM;

const float scale = 0.05;
const float bias = -0.025;

out vec4 fFragColor;

// LIGHT & MATERIAL VARIABLES

struct LightInfo {
	vec4 lightPos; // Posición de la luz (S.R. de la vista)
	vec3 intensity;
};
uniform LightInfo uLight;

struct MaterialInfo {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform MaterialInfo uMaterial;

vec3 ads(vec3 normal, vec3 viewDir, vec3 lightDir, vec2 vSToffset) {

    vec3 gloss = vec3(0.0, 0.0, 0.0);
    
    
    if(uApplyGloss == 1)
        gloss = texture(uGlossMap, vST).rgb;
    else
        gloss = vec3(0.5);

    if(uApplyCM == 1){      // PRESS C
        vec3 r = reflect(-lightDir, normal);
        return uLight.intensity * (  uMaterial.ambient +
                            uMaterial.diffuse * max(dot(lightDir, normal), 0.0) + 
                            uMaterial.specular *pow(max(dot(r, viewDir), 0), uMaterial.shininess));
    }
    else {
        vec3 r = reflect(-lightDir, normal);
        vec3 textureColor = texture(uColorMap , vSToffset).rgb;
        return uLight.intensity * (  textureColor * 0.2 +
                            textureColor * max(dot(lightDir, normal), 0.0) + 
                            gloss * pow(max(dot(r, viewDir), 0.0), uMaterial.shininess));
    }
}


void main()
{   
    vec3 viewDir = normalize(vViewDir);
    vec2 SToffset = vST;
    vec3 normal = vec3(0.0, 0.0, 1.0);

    switch(uApplyHM){       // PRESS P
        case 0:
            SToffset = vST;
            break;
        case 1:
            float height = texture(uHeightMap, vST).r;
            float hsb = height * scale + bias;
            SToffset = vST + hsb * viewDir.xy;
            break;
        case 2:
            float hsb1 = 1 * scale + bias;
            vec2 ST1 = vST + hsb1 * viewDir.xy;
            
            float hsb2 = 0 * scale + bias;
            vec2 ST2 = vST + hsb2 * viewDir.xy;

            for(int i = 1; i <= 5; ++i){
                SToffset = (ST1 + ST2) / 2;
                float hsb = (hsb1 + hsb2) / 2;
                float height = texture(uHeightMap, vST).r * scale + bias;
                if(height < hsb){
                    ST1 = SToffset;
                    hsb1 = hsb;
                }
                else{
                    // ... ???
                }
            }
            break;
    }

    if(uApplyNM == 1)       // PRESS M
        normal = normalize(2.0 * vec3(texture(uNormalMap, SToffset).xyz) - vec3(1.) );
    else
        normal = vec3(0.0, 0.0, 1.0);

    fFragColor = vec4(ads(normal, viewDir, normalize(vLightDir), SToffset), 1.);
}
