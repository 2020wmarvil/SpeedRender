#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 worldPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;  

uniform vec3 cameraPos;

void main() {
    // ambient color
    vec3 ambientColor = material.ambient * light.ambient;

    // diffuse color
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - worldPos);
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = material.diffuse * diffuseStrength * light.diffuse;

    // specular color
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = material.specular * specularStrength * light.specular;

    // compositing
    vec3 finalColor = ambientColor + diffuseColor + specularColor;
    
    FragColor = vec4(finalColor, 1.0);
}
