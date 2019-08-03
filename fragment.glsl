#version 330 core
out vec4 FragColor;

in vec3 outColor;
in vec3 inPos;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec2 offset;
uniform float opacity;


void main()
{

	//FragColor = texture(outTexture,newTexture)*vec4(outColor,1.0f);
	FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),opacity);

}

