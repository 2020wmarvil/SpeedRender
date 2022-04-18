#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 localNormal;

void main() {
    FragColor = vec4(normal, 1.0);
}
