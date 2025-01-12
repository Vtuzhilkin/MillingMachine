#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
	mat4 qt_Matrix;
	float qt_Opacity;	
	vec4 colorStart;
	vec4 colorEnd;
};
layout(binding = 1) uniform sampler2D source;

void main() {
    fragColor = mix(colorStart, colorEnd, (qt_TexCoord0.x + qt_TexCoord0.y)/2) * qt_Opacity;
}
