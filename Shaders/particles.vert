#version 330 core

// Specify the position and colors
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 life;

// Uniform variables sent over from CPU
uniform mat4 projection;
uniform mat4 view;

// Output goes to fragment shader
out vec3 particleColor;
out float remainingLife;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0f);
    float distanceFromCenter = length(vec3(projection * view * vec4(1.0f)));
    gl_PointSize = min(life.x / 10.0f, 25.0f);// * min(300.0f / distanceFromCenter, 1.0f);
    particleColor = color;
    remainingLife = life.x;
}
