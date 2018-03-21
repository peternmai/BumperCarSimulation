#version 330 core

// Specify the position and the normals
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec3 color;

// Variables passed from CPU
uniform mat4 worldToLightSpace;
uniform mat4 toWorld;

void main()
{
    gl_Position = worldToLightSpace * toWorld * vec4(position, 1.0);
}
