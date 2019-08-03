#version 330 core
out vec4 FragColor;

in vec3 outColor;
in vec3 inPos;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec2 offset;


void main()
{
    vec2 newTexture = TexCoord;
    float distance = 0.5f-sqrt(pow(offset.x - TexCoord.x,2) + pow(0.5f - TexCoord.y,2));

	//FragColor = texture(outTexture,newTexture)*vec4(outColor,1.0f);
	FragColor = texture(texture1,TexCoord)*distance;

}

