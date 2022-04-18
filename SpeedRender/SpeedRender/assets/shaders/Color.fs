#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 worldPos;
in vec2 texCoords;

uniform vec3 cameraPos;

void main() {
    vec3 color = vec3(texCoords.x, texCoords.y, 0.0);
    FragColor = vec4(color, 1.0);
}
