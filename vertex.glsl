#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 pos;
uniform mat3 anglesy;
uniform mat3 anglesx;

void main() {
    mat3 decreaser;
    decreaser[0] = vec3(0.2f,0,0);
    decreaser[1] = vec3(0,1.0,0);
    decreaser[2] = vec3(0,0,0.2f);

	gl_Position = vec4(decreaser*aPos.xyz*anglesx*anglesy,1.0f);
    pos = aPos;
}
