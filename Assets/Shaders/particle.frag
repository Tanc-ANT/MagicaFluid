#version 410

uniform mat4 projection;
uniform mat4 view;

in vec2 texcoord;
in vec4 color;
in float radius;

out vec4 outColor;


void main()
{
	vec3 N;
	N.xy = texcoord * 2.0 - 1.0;
	float r2 = dot(N.xy, N.xy);
	if (r2 > 1.0) discard;
	N.z = sqrt(1.0 - r2);
	
	outColor = color;
}