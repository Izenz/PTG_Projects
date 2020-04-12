#version 150 


in vec3 vECPos;
in vec3 vECNorm;
in vec3 vColor;
flat in vec3 flat_vColor;
flat in int SHADING_TYPE;
flat in int LIGHT_TYPE;

out vec4 fFragColor;

// LIGHT & MATERIAL VARIABLES

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

vec3 ads() {
    if(LIGHT_TYPE == 0){
        vec3 view = normalize(vec3(-vECPos));
        vec3 n = normalize(vECNorm);
        vec3 ldir = normalize(vec3( uLight.lightPos - vec4(vECPos, 1.0)));
        vec3 r = reflect(-ldir, n);
        return uLight.intensity * (  uMaterial.ambient +
                            uMaterial.diffuse * max(dot(ldir, n), 0.0) + 
                            uMaterial.specular *pow(max(dot(r, view), 0), uMaterial.shininess));
    }
    else{
        vec3 ldir = normalize(vec3( uLight.lightPos - vec4(vECPos, 1.0)));
	    float d = length(ldir);
	    ldir = normalize(ldir);
        vec3 n = normalize(vECNorm);
	    float angle = acos(dot(-ldir, normalize(uLight.dir)));
	    vec3 I = uLight.intensity / (uLight.k[0] + uLight.k[1]*d + uLight.k[2]*d*d);
	    vec3 ambient = I * uMaterial.ambient;
	    I *= 1 - smoothstep(radians(uLight.cutOffInt), radians(uLight.cutOffExt), angle);
	    vec3 r = normalize(reflect(-ldir, n));	
	    vec3 view = normalize(vec3(-vECPos));
	    return ambient + I * (	uMaterial.diffuse * max(dot(ldir, n), 0.0) +
							    uMaterial.specular * pow(max(dot(r,view),0), uMaterial.shininess));
    }
    
}

vec3 ads_comic() {
    vec3 ldir = normalize(vec3( uLight.lightPos - vec4(vECPos, 1.0)));
	vec3 view  = normalize( vec3(-vECPos) );
    vec3 n = normalize(vECNorm);
    vec3 r = reflect(-ldir, n);
    
    if((dot(normalize(view),normalize(n))) < 0.13)
        return vec3(0.0, 0.0, 0.0);

    float angle = dot(normalize(view), normalize(r));
    if(angle > 0.92)
        return uMaterial.specular + uMaterial.diffuse;
    else if((angle < 0.92) && ((dot(n, ldir)) > 0.0))
        return uMaterial.diffuse;
    else
        return uMaterial.ambient * 1.5;
    
    
    
    /*
    vec3 view = normalize(vec3(-vECPos));
    vec3 n = normalize(vECNorm);
    vec3 ldir = normalize(vec3( uLight.lightPos - vec4(vECPos, 1.0)));
    vec3 r = reflect(-ldir, n);

    

    float angle = dot(normalize(view), normalize(r));
    if(angle > 0.92)
        return uMaterial.specular + uMaterial.diffuse;
    else if((angle < 0.92) && (angle > 0.0))
        return uMaterial.diffuse;
    else
        return uMaterial.ambient * 1.5;
    */
}

const vec3 levels = vec3(3,3,2);

void main()
{   
    switch(SHADING_TYPE){
        case 0:
             fFragColor = vec4(vColor, 1.0);
            break;
        case 1:
             fFragColor = vec4(flat_vColor, 1.0);
            break;
        case 2:
            fFragColor = vec4(ads(), 1.0);
            break;
        case 3:
            vec3 color = ads_comic() * levels;
            color = clamp(round(color)/levels, 0.0, 1.0);
            fFragColor = vec4(color, 1.0);
    }
}
