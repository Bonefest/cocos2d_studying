#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 pos;
uniform mat3 anglesx;
uniform mat3 anglesz;

void main() {
	gl_Position = vec4(aPos.xyz,1.0f);
    pos = aPos;
}
