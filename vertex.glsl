#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 pos;
uniform mat3 anglesy;
uniform mat3 anglesx;

void main() {
	gl_Position = vec4(aPos.xyz*anglesx*anglesy,1.0f);
    pos = aPos;
}
