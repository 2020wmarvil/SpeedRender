// TODO: model->GetModelMatrix(); and scale each model appropriately
// TODO: wireframe shader

#define GLFW_INCLUDE_NONE
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
bool mainWindowFocused = true;

Camera camera(
    glm::vec3(5.0f),
    glm::vec3(-0.5f),
    glm::vec3(0.0f, 1.0f, 0.0f), WIDTH, HEIGHT
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
    Model sphere("assets/models/sphere.obj");
    Model bunny("assets/models/bunny.obj");
    Model teapot("assets/models/teapot.obj");
    Model suzanne("assets/models/suzanne.obj");

    // textures
    Texture::SetFlipImageOnLoad(true);
    Texture diffuseMap("assets/images/container2.png");
    Texture specularMap("assets/images/container2_specular.png");

    // set up shaders
    Shader unlitShader("assets/shaders/MainVertex.vs", "assets/shaders/Unlit.fs");
    Shader litShader("assets/shaders/MainVertex.vs", "assets/shaders/Lit.fs");
    Shader wireframeShader("assets/shaders/MainVertex.vs", "assets/shaders/Wireframe.fs");
    Shader normalsShader("assets/shaders/MainVertex.vs", "assets/shaders/TestNormals.fs");
    Shader uvsShader("assets/shaders/MainVertex.vs", "assets/shaders/TestUVs.fs");
    Material material = { diffuseMap, specularMap, 32.0f };

    // lights
    DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), Color(1.0f, 1.0f, 1.0f),
        { 
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        });

    enum ShaderState { SS_UNLIT, SS_LIT, SS_WIREFRAME, SS_NORMALS, SS_UVS, SS_COUNT };
    int shaderState = SS_WIREFRAME;
    Shader* shader = &wireframeShader;

    enum ModelState { MS_CUBE, MS_SPHERE, MS_BUNNY, MS_TEAPOT, MS_SUZANNE, MS_COUNT };
    int modelState = MS_BUNNY;
    Model* model = &bunny;

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

        ImGui::ShowDemoWindow();

        {
            ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);
            ImGui::Begin("Settings");
            ImGui::Text("Hold space to move around");

            const char* shader_names[SS_COUNT] = { "Unlit", "Lit", "Wireframe", "Normals", "UVs" };
            ImGui::SliderInt("Shader", &shaderState, 0, SS_COUNT - 1, shader_names[shaderState]);
            const char* model_names[MS_COUNT] = { "Cube", "Sphere", "Bunny", "Teapot", "Suzanne" };
            ImGui::SliderInt("Model", &modelState, 0, MS_COUNT - 1, model_names[modelState]);

            if (shaderState == SS_UNLIT) shader = &unlitShader;
            else if (shaderState == SS_LIT) shader = &litShader;
            else if (shaderState == SS_WIREFRAME) shader = &wireframeShader;
            else if (shaderState == SS_NORMALS) shader = &normalsShader;
            else if (shaderState == SS_UVS) shader = &uvsShader;

            if (modelState == MS_CUBE) model = &cube;
            else if (modelState == MS_SPHERE) model = &sphere;
            else if (modelState == MS_BUNNY) model = &bunny;
            else if (modelState == MS_TEAPOT) model = &teapot;
            else if (modelState == MS_SUZANNE) model = &suzanne;

            if (ImGui::Button("Fracture")) { }
            ImGui::SameLine();
            if (ImGui::Button("Reset")) { }
            ImGui::End();

            ImGui::Begin("FPS", (bool*)true, ImGuiWindowFlags_NoTitleBar);
            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
            ImGui::PopStyleColor();
        }

        glm::mat4 m = glm::mat4(1.0f);
        glm::mat4 v = camera.GetViewMatrix();
        glm::mat4 p = camera.GetProjectionMatrix();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuseMap.id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.specularMap.id);

        shader->Use();
        shader->SetMat4("model", m);
        shader->SetMat4("view", v);
        shader->SetMat4("projection", p);

        if (shaderState == SS_UNLIT) {
            shader->SetInt("mainTex", 0);
            shader->SetVec3("mainColor", glm::vec3(1.0f, 1.0f, 1.0f)); 
        } else if (shaderState == SS_LIT) {
            shader->SetVec3("cameraPos", camera.position);
            shader->SetVec3("dirLight.direction", dirLight.direction);
            shader->SetVec3("dirLight.color", dirLight.color);
            shader->SetVec3("dirLight.ambient", dirLight.lightProfile.ambient);
            shader->SetVec3("dirLight.diffuse", dirLight.lightProfile.diffuse);
            shader->SetVec3("dirLight.specular", dirLight.lightProfile.specular);
            shader->SetInt("material.diffuse", 0);
            shader->SetInt("material.specular", 1);
            shader->SetFloat("material.shininess", material.shininess);
        } else if (shaderState == SS_WIREFRAME) {
            shader->SetVec3("wireColor", glm::vec3(0.25f, 0.5f, 0.7f)); 
        }

        model->Draw(*shader);

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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        mainWindowFocused = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
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
