#version 330 core

out vec4 FragColor;

in vec2 texCoords;

void main() {
    vec3 color = vec3(texCoords.x, texCoords.y, 0.0);
    FragColor = vec4(texCoords.xy, 0.0, 1.0);
}
