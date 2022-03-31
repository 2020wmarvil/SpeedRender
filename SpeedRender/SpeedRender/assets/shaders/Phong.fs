#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 worldPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

void main() {
    // base color
    vec3 baseColor = vec3(1.0, 1.0, 0.0);

    // ambient color
    float ambientStrength = 0.1;
    vec3 ambientColor = ambientStrength * lightColor;

    // diffuse color
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - worldPos);
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = diffuseStrength * lightColor;

    // specular color
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float specularIntensity = 0.5;
    float gloss = 256;
    float specularStrength = specularIntensity * pow(max(dot(viewDir, reflectDir), 0.0), gloss);
    vec3 specularColor = specularStrength * lightColor;

    // compositing
    vec3 finalColor = baseColor * (ambientColor + diffuseColor + specularColor);
    
    FragColor = vec4(finalColor, 1.0);
}
