#version 410
in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 coord;

void main() {
    gl_Position = projection * view * vec4(position, 1.0);
    coord = position;
}  