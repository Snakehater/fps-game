#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// sampler2D holds texture id that we have assigned with glUniform1i and activated with glActiveTexture in main.cpp
uniform sampler2D ourTexture;

void main()
{
//	FragColor = vec4( 1.0, 0.5f, 0.2f, 1.0f );
	FragColor = texture( ourTexture, TexCoord );
}
