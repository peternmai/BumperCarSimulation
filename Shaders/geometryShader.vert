#version 330 core

// Specify the position and the normals
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec3 color;

// Uniform variables sent over from CPU
uniform mat4 projection;
uniform mat4 modelview;

// Outputs of vertex shader to be sent to fragment shader
out vec3 vertexNormal;
out vec3 vertexColor;
out vec3 vertexPosition;
out mat4 modelView;

void main()
{
    gl_Position = projection * modelview * vec4(position, 1.0f);
    vertexNormal = normals;
    vertexColor = color;
    vertexPosition = position;
    modelView = modelview;
}
