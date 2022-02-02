#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float TexId;
in vec3 Color;

// sampler2D holds texture id that we have assigned with glUniform1i and activated with glActiveTexture in main.cpp
uniform sampler2D ourTexture;

void main()
{
	FragColor = mix(texture(ourTexture, TexCoord), vec4(Color, 0.5f), 0.5f);
}
