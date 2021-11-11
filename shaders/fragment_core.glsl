#version 330 core
out vec4 FragColor;

in vec3 ourColor; // we set this variable in the OpenGL code.

void main()
{
//	FragColor = vec4( 1.0, 0.5f, 0.2f, 1.0f );
	FragColor = vec4( ourColor, 1.0f );
}
