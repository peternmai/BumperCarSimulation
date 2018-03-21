#version 330 core

uniform vec3 ucolor;
out vec4 color;

void main() {
  color = vec4(ucolor, 1.0f);
}