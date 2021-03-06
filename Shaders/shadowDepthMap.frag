#version 330 core

// Inputs from CPU
uniform float zNear;
uniform float zFar;

// Input from vertex shader
in vec4 shadowCoordinate;

// Shadow map
uniform sampler2D shadowMap;

// Output the color of the current fragment
out vec4 color;

void main()
{
    float c = texture(shadowMap, shadowCoordinate.xy).x;
    color = vec4(c,c,c,1.0f);
}
