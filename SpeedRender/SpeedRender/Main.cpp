#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Core.h"
#include "Lighting.h"
#include "Shader.h"
#include "Texture.h"

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#define WIDTH 800
#define HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float lastTime = 0.0f;
float deltaTime = 0.0f;

float pitch = 0.0f;
float yaw = -90.0f;

float lastX = WIDTH / 2, lastY = HEIGHT / 2;
bool firstMouse = true;

float fov = 45.0f;

int main() {
    // initialization
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window creation
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Speed Render", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }   

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glfwSetCursorPosCallback(window, mouse_callback);  
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetScrollCallback(window, scroll_callback); 

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // mesh data
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
    
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
    
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    glm::vec3 cubePositions[] = {
       glm::vec3( 0.0f,  0.0f,  0.0f), 
       glm::vec3( 2.0f,  5.0f, -15.0f), 
       glm::vec3(-1.5f, -2.2f, -2.5f),  
       glm::vec3(-3.8f, -2.0f, -12.3f),  
       glm::vec3( 2.4f, -0.4f, -3.5f),  
       glm::vec3(-1.7f,  3.0f, -7.5f),  
       glm::vec3( 1.3f, -2.0f, -2.5f),  
       glm::vec3( 1.5f,  2.0f, -2.5f), 
       glm::vec3( 1.5f,  0.2f, -1.5f), 
       glm::vec3(-1.3f,  1.0f, -1.5f)  
}   ;

    // set up buffers
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);  
    // tex coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);  

    // textures
    Texture::SetFlipImageOnLoad(true);
    Texture diffuseMap("assets/images/container2.png");
    Texture specularMap("assets/images/container2_specular.png");

    // set up shaders
    Shader shader("assets/shaders/MainVertex.vs", "assets/shaders/Phong.fs");
    Material material = { diffuseMap, specularMap, 32.0f };

    // lights
    DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), Color(1.0f, 0.0f, 0.0f),
        { 
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        });

    glm::vec3 pointLightPositions[] = {
    	glm::vec3( 0.7f,  0.2f,  2.0f),
    	glm::vec3( 2.3f, -3.3f, -4.0f),
    	glm::vec3(-4.0f,  2.0f, -12.0f),
    	glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    AttenuationProfile ap = { 1.0f, 0.09, 0.032f };

    PointLight light(pointLightPositions[0], glm::vec3(0.1f, 0.1f, 0.1f), Color(0.0f, 1.0f, 1.0f), 
        { 
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        }, ap);
    PointLight light2(pointLightPositions[1], glm::vec3(0.1f, 0.1f, 0.1f), Color(0.0f, 0.0f, 1.0f), 
        { 
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        }, ap);
    PointLight light3(pointLightPositions[2], glm::vec3(0.1f, 0.1f, 0.1f), Color(1.0f, 1.0f, 0.0f), 
        { 
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        }, ap);
    PointLight light4(pointLightPositions[3], glm::vec3(0.1f, 0.1f, 0.1f), Color(1.0f, 0.0f, 1.0f), 
        { 
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        }, ap);

    SpotLight spot1(cameraPos, cameraFront, 10.0f, 7.5f, 12.5f, glm::vec3(0.1f, 0.1f, 0.1f), Color(0.0f, 1.0f, 1.0f), 
        { 
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        }, 

        { 1.0f, 0.09f, 0.032f }
        
        );

    glm::vec3 initPos = cameraPos;
    glm::vec3 initDir = cameraFront;

    // render loop
    while(!glfwWindowShouldClose(window)) {
        deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        shader.Use();
        shader.SetMat4("model", model);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);
        shader.SetVec3("cameraPos", cameraPos);
        shader.SetInt("material.diffuse", 0);
        shader.SetInt("material.specular", 1);
        shader.SetFloat("material.shininess", material.shininess);

        shader.SetVec3("pointLights[0].position", light.position);
        shader.SetVec3("pointLights[0].color", light.color);
        shader.SetVec3("pointLights[0].ambient",  light.lightProfile.ambient);
        shader.SetVec3("pointLights[0].diffuse",  light.lightProfile.diffuse);
        shader.SetVec3("pointLights[0].specular", light.lightProfile.specular);
        shader.SetFloat("pointLights[0].constant",  light.attenuationProfile.constant);
        shader.SetFloat("pointLights[0].linear",    light.attenuationProfile.linear);
        shader.SetFloat("pointLights[0].quadratic", light.attenuationProfile.quadratic);

        shader.SetVec3("pointLights[1].position", light2.position);
        shader.SetVec3("pointLights[1].color", light2.color);
        shader.SetVec3("pointLights[1].ambient",  light2.lightProfile.ambient);
        shader.SetVec3("pointLights[1].diffuse",  light2.lightProfile.diffuse);
        shader.SetVec3("pointLights[1].specular", light2.lightProfile.specular);
        shader.SetFloat("pointLights[1].constant",  light2.attenuationProfile.constant);
        shader.SetFloat("pointLights[1].linear",    light2.attenuationProfile.linear);
        shader.SetFloat("pointLights[1].quadratic", light2.attenuationProfile.quadratic);

        shader.SetVec3("pointLights[2].position", light3.position);
        shader.SetVec3("pointLights[3].color", light3.color);
        shader.SetVec3("pointLights[2].ambient",  light3.lightProfile.ambient);
        shader.SetVec3("pointLights[2].diffuse",  light3.lightProfile.diffuse);
        shader.SetVec3("pointLights[2].specular", light3.lightProfile.specular);
        shader.SetFloat("pointLights[2].constant",  light3.attenuationProfile.constant);
        shader.SetFloat("pointLights[2].linear",    light3.attenuationProfile.linear);
        shader.SetFloat("pointLights[2].quadratic", light3.attenuationProfile.quadratic);

        shader.SetVec3("pointLights[3].position", light4.position);
        shader.SetVec3("pointLights[3].color", light4.color);
        shader.SetVec3("pointLights[3].ambient",  light4.lightProfile.ambient);
        shader.SetVec3("pointLights[3].diffuse",  light4.lightProfile.diffuse);
        shader.SetVec3("pointLights[3].specular", light4.lightProfile.specular);
        shader.SetFloat("pointLights[3].constant",  light4.attenuationProfile.constant);
        shader.SetFloat("pointLights[3].linear",    light4.attenuationProfile.linear);
        shader.SetFloat("pointLights[3].quadratic", light4.attenuationProfile.quadratic);

        shader.SetVec3("dirLight.direction", dirLight.direction);
        shader.SetVec3("dirLight.color", dirLight.color);
        shader.SetVec3("dirLight.ambient", dirLight.lightProfile.ambient);
        shader.SetVec3("dirLight.diffuse", dirLight.lightProfile.diffuse);
        shader.SetVec3("dirLight.specular", dirLight.lightProfile.specular);

        spot1.position = cameraPos;
        spot1.direction = cameraFront;

        shader.SetVec3("spotLights[0].position", spot1.position);
        shader.SetVec3("spotLights[0].color", spot1.color);
        shader.SetVec3("spotLights[0].direction", spot1.direction);
        shader.SetFloat("spotLights[0].cutoff", spot1.cutoff);
        shader.SetFloat("spotLights[0].innerCutoff", spot1.innerCutoff);
        shader.SetFloat("spotLights[0].outerCutoff", spot1.outerCutoff);
        shader.SetVec3("spotLights[0].ambient", spot1.lightProfile.ambient);
        shader.SetVec3("spotLights[0].diffuse", spot1.lightProfile.diffuse);
        shader.SetVec3("spotLights[0].specular", spot1.lightProfile.specular);
        shader.SetFloat("spotLights[0].constant", spot1.attenuationProfile.constant);
        shader.SetFloat("spotLights[0].linear", spot1.attenuationProfile.linear);
        shader.SetFloat("spotLights[0].quadratic", spot1.attenuationProfile.quadratic);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuseMap.id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.specularMap.id);

        glBindVertexArray(VAO);
        for(unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i; 
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        light.Draw(view, projection);
        light2.Draw(view, projection);
        light3.Draw(view, projection);
        light4.Draw(view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();

        glCheckError();
    }

    // cleanup
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}  

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 2.0f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos -= cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos += cameraUp * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}  

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 90.0f)
        fov = 90.0f; 
}
