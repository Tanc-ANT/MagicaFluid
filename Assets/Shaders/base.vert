#version 410 core

uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in float radius;

out vec4 gsColor;
out float gsRadius;

void main()
{
	vec3 posEye = vec3(view * vec4(position.xyz, 1.0));
    float dist = length(posEye);
	gl_PointSize = 250.0/dist;
    gl_Position = projection *view * vec4(position, 1.0);
	gsColor = color;
	gsRadius = radius;
}