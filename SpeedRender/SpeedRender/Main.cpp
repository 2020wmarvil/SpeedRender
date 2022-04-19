﻿#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Core.h"
#include "Camera.h"
#include "Lighting.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"

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
void ProcessInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float lastTime = 0.0f, deltaTime = 0.0f;
bool testNormals = false, oneIsPressed = false;
bool mainWindowFocused = true;

Camera camera(
    glm::vec3(0.0f, 0.0f,  3.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f,  0.0f), WIDTH, HEIGHT
);

int main() {
    // initialization
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window creation
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Speed Render", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glfwSetCursorPosCallback(window, mouse_callback);  
    glfwSetScrollCallback(window, scroll_callback); 

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // setup imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // mesh data
    Model cube("assets/models/cube.obj");

    // textures
    Texture::SetFlipImageOnLoad(true);
    Texture diffuseMap("assets/images/container2.png");
    Texture specularMap("assets/images/container2_specular.png");

    // set up shaders
    Shader normalsShader("assets/shaders/MainVertex.vs", "assets/shaders/TestNormals.fs");
    Shader uvsShader("assets/shaders/MainVertex.vs", "assets/shaders/TestUVs.fs");
    Material material = { diffuseMap, specularMap, 32.0f };

    // lights
    DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), Color(1.0f, 0.0f, 0.0f),
        { 
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        });

    // render loop
    while(!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        ProcessInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            //ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Fracture")) {
                //Fracture();
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();

        /* Lit Shader 
        shader.SetMat4("model", model);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);
        shader.SetVec3("cameraPos", camera.position);
        shader.SetVec3("dirLight.direction", dirLight.direction);
        shader.SetVec3("dirLight.color", dirLight.color);
        shader.SetVec3("dirLight.ambient", dirLight.lightProfile.ambient);
        shader.SetVec3("dirLight.diffuse", dirLight.lightProfile.diffuse);
        shader.SetVec3("dirLight.specular", dirLight.lightProfile.specular);
        shader.SetInt("material.diffuse", 0);
        shader.SetInt("material.specular", 1);
        shader.SetFloat("material.shininess", material.shininess); */

        /* Unlit Shader
        shader.SetMat4("model", model);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);
        shader.SetVec3("mainColor", glm::vec3(1.0f, 1.0f, 1.0f)); */

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuseMap.id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.specularMap.id);

        if (testNormals) {
            normalsShader.Use();
            normalsShader.SetMat4("model", model);
            normalsShader.SetMat4("view", view);
            normalsShader.SetMat4("projection", projection);
            cube.Draw(normalsShader);
        } else {
            uvsShader.Use();
            uvsShader.SetMat4("model", model);
            uvsShader.SetMat4("view", view);
            uvsShader.SetMat4("projection", projection);
            cube.Draw(uvsShader);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();

        glCheckError();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void ProcessInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!oneIsPressed) {
            testNormals = !testNormals;
            oneIsPressed = true;
        }
    } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        oneIsPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        mainWindowFocused = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        mainWindowFocused = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  
    }

    if (mainWindowFocused) camera.ProcessWindowEvents(window, deltaTime);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}  

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mainWindowFocused) camera.UpdateMousePosition(xpos, ypos);
}  

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (mainWindowFocused) camera.UpdateScrollPosition(xoffset, yoffset);
}
