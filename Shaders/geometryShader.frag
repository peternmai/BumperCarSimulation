#version 330 core

// Inputs sent over from vertex shader
in vec3 vertexNormal;

// Output the color of the current fragment
out vec4 color;

void main()
{
    color = vec4(vertexNormal * 0.5 + 0.5, 1.0f);
}
