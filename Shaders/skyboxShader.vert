#version 330 core

layout (location = 0) in vec3 position;

out vec3 textureCoordinates;

// Uniform variables sent over from CPU
uniform mat4 projection;
uniform mat4 view;

void main()
{
    textureCoordinates = position;
    gl_Position = projection * view * vec4(position, 1.0);
}
