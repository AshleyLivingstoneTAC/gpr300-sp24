#version 450
out vec4 FragColor; //The color of this fragment
in vec2 UV;


in Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; //Vertex normal in world space
	vec2 TexCoord;
	mat3 TBN;
}fs_in;

in vec4 LightSpacePos;
uniform sampler2D _MainTex;
uniform sampler2D _NormalMap;
uniform sampler2D _ShadowMap; 
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);


struct Material{
	float Ka; //Ambient coefficient (0-1)
	float Kd; //Diffuse coefficient (0-1)
	float Ks; //Specular coefficient (0-1)
	float Shininess; //Affects size of specular highlight
};

struct PointLight{
	vec3 position;
    float radius;
	vec4 color;
};
#define MAX_POINT_LIGHTS 64
uniform PointLight _PointLights[MAX_POINT_LIGHTS];

uniform Material _Material;
uniform layout(binding = 0) sampler2D _gPositions;
uniform layout(binding = 1) sampler2D _gNormals;
uniform layout(binding = 2) sampler2D _gAlbedo;

float attenuateLinear(float distance, float radius)
{
	return clamp(((radius-distance)/radius),0.0,1.0);
}

float calcShadow(sampler2D shadowMap, vec4 lightSpacePos)
{
//Homogeneous Clip space to NDC [-w,w] to [-1,1]
    vec3 sampleCoord = lightSpacePos.xyz / lightSpacePos.w;
    //Convert from [-1,1] to [0,1]
    sampleCoord = sampleCoord * 0.5 + 0.5;
	float myDepth = sampleCoord.z; 
float shadowMapDepth = texture(shadowMap, sampleCoord.xy).r;
//step(a,b) returns 1.0 if a >= b, 0.0 otherwise
 return step(shadowMapDepth,myDepth);

}
vec3 calcPointLight(PointLight light,vec3 normal){
	vec3 diff = light.position - fs_in.WorldPos;
	//Direction toward light position
	vec3 toLight = normalize(diff);
	//TODO: Usual blinn-phong calculations for diffuse + specular
	vec3 lightColor = (diff + _Material.Ks) * light.color.rgb;
	//Attenuation
	float d = length(diff); //Distance to light
	lightColor*=attenuateLinear(d,light.radius); //See below for attenuation options
	return lightColor;
}

vec3 calculateLighting(vec3 n, vec3 pos, vec3 a)
{
	vec3 light = vec3(n * pos * a);
	return light;
}

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 totalLight = vec3(0);
	vec3 albedo = texture(_MainTex,fs_in.TexCoord).xyz;
	totalLight += calculateLighting(normal, _PointLights[0].position, albedo);
	for(int i=0;i<MAX_POINT_LIGHTS;i++)
	{
    totalLight+=calcPointLight(_PointLights[i],normal);
    }
FragColor = vec4(albedo * totalLight,0);
}
