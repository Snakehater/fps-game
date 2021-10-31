#version 330 core

layout(location=0) in vec3 in_pos;
layout(location=1) in vec3 vertexColor;
layout(location=2) in vec2 vertexUV;
layout(location=3) in vec3 normal;

uniform mat4 fullTransformMatrix;


uniform vec3 ambientLight;
uniform vec3 lightPosition;

out vec3 theColor;
out vec2 UV;

void main()
{
    vec4 v = vec4(in_pos, 1.0);
    gl_Position = fullTransformMatrix* v;

    vec3 lightVector = normalize(lightPosition-normal);
    float brightness = dot(lightVector,normal);
    theColor = vertexColor * vec3( brightness, brightness, brightness);// * ambientLight;

    UV = vertexUV;
}
