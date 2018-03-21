#version 330

// Input from vertex shaders
in vec3 particleColor;
in float remainingLife;

// Output the color of the current fragment
out vec4 color;

void main()
{
    color = vec4(particleColor, 1.0f);
    color.a = min(remainingLife / 10.0f, 0.5);
}
