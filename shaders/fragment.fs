#version 330 core
out vec4 color;

in vec3 theColor;
in vec2 UV;

uniform sampler2D myTextureSampler;

void main()
{
    color = texture(myTextureSampler,UV);
    //texture(myTextureSampler,UV);
    // * vec4(theColor,1.0);

}
