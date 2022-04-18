#version 330 core

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D mainTex;
uniform vec3 mainColor;

void main() {
    vec4 outColor = texture(mainTex, texCoords) * vec4(mainColor, 1.0);
    FragColor = outColor;
}
