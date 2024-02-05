#version 450
layout(location = 0) out vec4 FragColor;
 mat3 boxBlur = mat3(1, 1, 1, 1, 1, 1, 1, 1, 1) * 0.1111;
 mat3 gaussian = mat3(1, 2, 1, 2, 4, 2, 1, 2, 1) * 0.0625;
 mat3 sharpen = mat3(0, -1, 0, -1, 5, -1, 0, -1, 0);
 mat3 edge = mat3(-1, -1, -1, -1, 8, -1, -1, -1, -1);
 in vec2 UV;
 uniform sampler2D _ColorBuffer;
 uniform float proc;
void main()
{
vec2 texelSize = proc / textureSize(_ColorBuffer,0).xy;
vec3 totalColor = vec3(0);
for(int y = -2; y <= 2; y++){
   for(int x = -2; x <= 2; x++){
      vec2 offset = vec2(x,y) * texelSize;
      totalColor += texture(_ColorBuffer,UV + offset).rgb;
   }
}
totalColor/=(5 * 5);
FragColor = vec4(totalColor,1.0); 

}