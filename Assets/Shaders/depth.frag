#version 410

uniform mat4 projection;
uniform mat4 view;

in vec2 texcoord;
in vec4 color;
in vec3 eyepos;
in float radius;

uniform float colorThickness = 0.0;
uniform int renderMode;
uniform float near = 1.0f;
uniform float far = 50.0f;

out vec4 outColor;

void main()
{
	vec3 N;
	N.xy = texcoord * 2.0 - 1.0;
	float r2 = dot(N.xy, N.xy);
	if (r2 > 1.0) discard;
	N.z = sqrt(1.0 - r2);
	
	// calculate depth
	//vec4 pixelPos = vec4(eyepos + N * radius, 1.0);
	//vec4 clipSpacePos = projection * pixelPos;
	//gl_FragDepth = (clipSpacePos.z / clipSpacePos.w) * 0.5f + 0.5f;
	
	//vec4 fragPos = vec4(eyepos + N * radius, 1.0);
	//vec4 clipPos = projection * fragPos;
	//float z = clipPos.z / clipPos.w;
	//gl_FragDepth = z;
	
	float z = eyepos.z + N.z * radius;
	z = far * (z + near) / (z * (far - near));
	gl_FragDepth = z * 0.5;
}