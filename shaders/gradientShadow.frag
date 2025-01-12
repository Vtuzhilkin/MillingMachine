#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
	mat4 qt_Matrix;
	float qt_Opacity;	
	vec4 colorStart;
	vec4 colorCenter;
	vec4 colorEnd;
	int coefGradient;
};
layout(binding = 1) uniform sampler2D source;

void main() {
	float k = coefGradient;
	float xt = 2/(1+exp(-k*qt_TexCoord0.x))+2/(1+exp(k*(qt_TexCoord0.x-1)))-3;
	vec4 colorx;
	if(qt_TexCoord0.x <= 0.5){
		colorx = mix(colorStart, colorCenter, xt);
	}else{
		colorx = mix(colorEnd, colorCenter, xt);		
	}

	float yt = 2/(1+exp(-k*qt_TexCoord0.y))+2/(1+exp(k*(qt_TexCoord0.y-1)))-3;
	vec4 colory;
	if(qt_TexCoord0.y <= 0.5){
		colory = mix(colorStart, colorCenter, yt);
	}else{
		colory = mix(colorEnd, colorCenter, yt);		
	}
	
	fragColor = (colorx + colory)/2 * texture(source, qt_TexCoord0).a * qt_Opacity;

}
