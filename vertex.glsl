#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 textCord;

out vec2 pos;
out vec2 texCord;

uniform mat4 transform;


void main() {
	gl_Position = vec4(transform*vec4(aPos,0.0f,1.0f));
    pos = aPos;
    texCord = vec2(textCord.x,textCord.y);
}
