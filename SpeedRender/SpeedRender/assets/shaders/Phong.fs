#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 worldPos;
in vec2 texCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
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
    vec3 ambientColor = light.ambient * vec3(texture(material.diffuse, texCoords));

    // diffuse color
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - worldPos);
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = light.diffuse * diffuseStrength * vec3(texture(material.diffuse, texCoords));  

    // specular color
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = light.specular * specularStrength * vec3(texture(material.specular, texCoords));

    // compositing
    vec3 finalColor = ambientColor + diffuseColor + specularColor;
    
    FragColor = vec4(finalColor, 1.0);
}
