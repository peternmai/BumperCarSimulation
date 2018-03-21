#version 330 core

// Specify the position and the normals
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec3 color;

// Uniform variables sent over from CPU
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 toWorld;
uniform mat4 shadowDepthBias;
uniform mat4 worldToLightSpace;

// Outputs of vertex shader to be sent to fragment shader
out vec3 vertexNormal;
out vec3 vertexColor;
out vec3 FragPos;
out vec3 vertexPosition;
out mat4 modelView;
out vec4 shadowCoordinate;

void main()
{
    gl_Position = projection * modelview * vec4(position, 1.0f);
    vertexNormal = normals;
    vertexColor = color;

	// Sending info about shadows
    FragPos = vec3(modelview * vec4(position, 1.0));
    vertexPosition = position;
    modelView = modelview;
    shadowCoordinate = shadowDepthBias * toWorld * vec4(position, 1.0f);
}
