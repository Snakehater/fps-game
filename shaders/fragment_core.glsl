#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
flat in int TexId;
in vec3 Color;

// sampler2D holds texture id that we have assigned with glUniform1i and activated with glActiveTexture in main.cpp
uniform sampler2D texture1;
uniform sampler2D texture7;
// Array of textures, max size is 128
uniform sampler2D[128] textures;

void main()
{
	//FragColor = mix(texture(ourTexture, TexCoord), vec4(Color, 0.5f), 0.5f);
	//FragColor = texture(texture1, TexCoord);
	FragColor = vec4(Color, 0.5f);
}
