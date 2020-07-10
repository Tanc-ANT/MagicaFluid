#version 400 core

uniform mat4 projection;
uniform mat4 view;

in vec2 texcoord;
in vec4 color;
in vec3 eyepos;
in float radius;

out float thickness;

void main() {
	//calculate normal
	vec3 normal;
	normal.xy = texcoord * 2.0 - 1.0;
	float r2 = dot(normal.xy, normal.xy);
	
	if (r2 > 1.0f) {
		discard;
	}
	
	normal.z = sqrt(1 - r2);
	
	thickness = normal.z * 0.005f;
}