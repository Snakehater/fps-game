#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// sampler2D holds texture id that we have assigned with glUniform1i and activated with glActiveTexture in main.cpp
uniform sampler2D ourTexture;

void main()
{
	FragColor = texture( ourTexture, TexCoord );
}
