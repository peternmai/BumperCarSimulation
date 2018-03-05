#version 330 core

// Specify the position
layout (location = 0) in vec3 position;

// Uniform variables sent over from CPU
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0);
}
