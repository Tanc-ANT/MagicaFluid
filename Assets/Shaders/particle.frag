#version 410

uniform mat4 projection;
uniform mat4 view;
uniform float near=0.1;
uniform float far=100;

in vec2 texcoord;
in vec4 color;
in vec3 eyepos;
in float radius;

out vec4 outColor;


void main()
{
	vec3 N;
	N.xy = texcoord * 2.0 - 1.0;
	float r2 = dot(N.xy, N.xy);
	if (r2 > 1.0) discard;
	N.z = sqrt(1.0 - r2);
	
	vec4 pixelPos = vec4(eyepos + N * radius, 1.0);
	vec4 clipSpacePos = projection * pixelPos;
	gl_FragDepth = (clipSpacePos.z / clipSpacePos.w) * 0.5f + 0.5f;
	
	outColor = color;
}