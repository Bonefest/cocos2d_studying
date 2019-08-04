#version 330 core
out vec4 FragColor;
in vec3 pos;

void main()
{

    FragColor = vec4(1.0f-pos.x,1.0f-pos.y,1.0f-pos.z,1.0f);
}

