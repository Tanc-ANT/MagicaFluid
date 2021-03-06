#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in float radius;

uniform mat4 projection;
uniform mat4 view;

out vec4 gsColor;
out float gsRadius;

void main()
{
	gl_Position = view * vec4(position, 1.0);
	gsColor = color;
	gsRadius = radius;
	gl_PointSize = radius / gl_Position.w;
}