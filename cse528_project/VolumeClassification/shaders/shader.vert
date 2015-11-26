#version 330 core
layout(location = 0)

in vec3 vert_position;
in vec3 vert_texcoord;

out vec3 frag_texcoord;

uniform mat4 view;
uniform mat4 proj;

void main() {
	gl_Position = proj * view * vec4(vert_position, 1);

	frag_texcoord = vert_texcoord;
}
