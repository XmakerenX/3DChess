#version 330 core

#define MAX_ACTIVE_LIGHTS 4

uniform Material
{
    vec4 mDiffuse;
    vec4 mAmbient;
    vec4 mSpecular;
    vec4 mEmissive;
    float mPower;
};

struct LightData
{
	// position and orientation
	vec3 pos;
	vec4 direction;
	vec3 Attenuation012;
	// color
	vec4 lAmbient;
	vec4 lDiffuse;
	vec4 lSpecular;
	float outerCutoff;
	float innerCutoff;
};

layout(std140) uniform lightBlock
{
	LightData lights[4];
};

uniform mat4 matWorldInverseT;
uniform mat4 matWorld;

uniform vec3 vecEye;

uniform int nActiveLights;
uniform bool textured;

in vec3 pos;
in vec3 norm;

out vec4 color;

vec4 calcLight(int i, vec3 posW, vec3 norm, vec3 viewDir)
{
		vec3 lightDir;
		if (lights[i].pos.x != 0 || lights[i].pos.y != 0 || lights[i].pos.z != 0)
			lightDir = normalize(lights[i].pos - posW);
		else
			lightDir = (normalize(-lights[i].direction)).xyz;
		
		float diff = max( dot( norm, lightDir), 0);
		
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), mPower);
		
		float distance = length(lights[i].pos - posW);
		
		float attenuation;
		float intensity;
		
		if (lights[i].pos.x != 0 || lights[i].pos.y != 0 || lights[i].pos.z != 0)
		{
			attenuation = lights[i].Attenuation012.x;
			attenuation = attenuation + lights[i].Attenuation012.y * distance;
 			attenuation = attenuation + lights[i].Attenuation012.z * distance * distance;
 			
 			if (lights[i].outerCutoff != 0)
 			{
 				float theta = dot(lightDir, normalize(-lights[i].direction).xyz);
 				float eps = lights[i].innerCutoff - lights[i].outerCutoff;
 
 				intensity = clamp( ((theta -  lights[i].outerCutoff) / eps) , 0.0, 1.0);
 			}
 			else
				intensity = 1;
		}
		else
		{
			attenuation = 1;
 			intensity = 1;
		}

 		vec3 ambient  = (lights[i].lAmbient * mAmbient).rgb;
  		vec3 diffuse = (lights[i].lDiffuse * diff * mDiffuse).rgb;
  		vec3 specular = (lights[i].lSpecular * spec * mSpecular).rgb;
 		
 		diffuse  *= intensity / attenuation;
 		specular *= intensity / attenuation;
 		
 		return vec4(ambient + diffuse + specular,1.0);
}

void main()
{
 	vec3 posW = (matWorld * vec4(pos, 1.0)).xyz;
 	vec3 norm = (matWorldInverseT * vec4(norm, 0.0)).xyz;
 	norm = normalize(norm);

 	vec3 viewDir = normalize(vecEye - posW);
 	vec4 outColor = vec4(0.0, 0.0, 0.0, 0.0);
 	
	color = vec4(0.0,0.0,0.0,1.0);
	
	int activeLights = min(nActiveLights, MAX_ACTIVE_LIGHTS);
	for (int i = 0; i < activeLights; i++)
	{
		outColor += calcLight(i, posW, norm, viewDir);
	}
	color = outColor;
}
