#version 330 core

// Inputs sent over from vertex shader
in vec3 vertexNormal;
in vec3 vertexColor;
in vec3 FragPos;

uniform vec3 eyePos;
uniform int fog;

// Output the color of the current fragment
out vec4 color;

void main()
{
    //color = vec4(vertexNormal * 0.5 + 0.5, 1.0f);
    
    color = vec4(vertexColor, 1.0f);


    float fogEnd = 200.0f;
    float fogStart = 20.0f;
    vec3 fogColor = vec3(0.5, 0.5, 0.5);
    if(fog == 1) {
      vec3 colorwfog = vec3(0.0, 0.0, 0.0);
      float fogFactor = (fogEnd - abs(FragPos.z)) / (fogEnd - fogStart);
      fogFactor = clamp (fogFactor, 0.0, 1.0);

      colorwfog = mix(fogColor, vertexColor, fogFactor);

      color = vec4(colorwfog, 1.0);
    }
}
