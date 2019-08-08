#version 330 core
out vec4 FragColor;
in vec3 pos;

void main()
{
    float com = sqrt(pos.x * pos.x + pos.y*pos.y + pos.z*pos.z);
    //float com = 1.0f;
    FragColor = vec4((1+pos.x)/2,(1+pos.y)/2,(1+pos.z)/2,1.0f);
    //FragColor = vec4(com,com,com,1.0f);
}

