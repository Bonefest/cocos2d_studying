#version 330 core
out vec4 FragColor;
in vec2 pos;
in vec2 texCord;

uniform sampler2D texture1;


void main()
{
    //float com = 1.0f;
    FragColor = texture(texture1,texCord);
    //FragColor = vec4(1.0f,1.0,1.0f,1.0f);
}

