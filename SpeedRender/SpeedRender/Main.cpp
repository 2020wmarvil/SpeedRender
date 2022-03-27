#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define WIDTH 640
#define HEIGHT 480

float vertices[] = {
    -0.5f, -0.5f, 0.5f,
     0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,

    -0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
     0.5f, 0.5f, -0.5f,
}; 

int indices[] = {
    0, 3, 2,
    0, 1, 3,

    1, 8, 3,
    1, 6, 8,

    6, 7, 8,
    6, 5, 7,

    5, 0, 2,
    5, 2, 7,

    5, 6, 1,
    5, 1, 0,

    7, 8, 3,
    7, 3, 2,
};

static const char* vertex_shader_text =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"    gl_Position = mvp * vec4(position, 1.0);\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 110\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 proj;

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

struct Params {
    uint16_t state;

    uint16_t W_DOWN = 1 << 0;
    uint16_t A_DOWN = 1 << 1;
    uint16_t S_DOWN = 1 << 2;
    uint16_t D_DOWN = 1 << 3;
};

Params params;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::cout << "key action ";
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) { params.state |= params.A_DOWN; }
        else if (action == GLFW_RELEASE) params.state &= ~params.A_DOWN;
    } else if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) { params.state |= params.D_DOWN; }
        else if (action == GLFW_RELEASE) params.state &= ~params.D_DOWN;
    } else if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) { params.state |= params.S_DOWN; }
        else if (action == GLFW_RELEASE) params.state &= ~params.S_DOWN;
    } else if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) { params.state |= params.W_DOWN; }
        else if (action == GLFW_RELEASE) params.state &= ~params.W_DOWN;
    }
    std::cout << params.state << std::endl;
}

int main(void) {
	if (!glfwInit()) {
		std::cout << "GLFW Init failed" << std::endl;
	}

	glfwSetErrorCallback(error_callback);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "My Title", NULL, NULL);
	if (!window) {
		std::cout << "Window creation failed" << std::endl;
	}
	glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

	glfwSetKeyCallback(window, key_callback);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

	glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
 
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
 
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);

    glm::mat4 view = glm::lookAt(
        cameraPos,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    float lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        float moveAmount = 3.0f;

        glm::vec3 movement(0.0f);

        if (params.state & params.W_DOWN) {
            movement.x += moveAmount * deltaTime;
        } else if (params.state & params.S_DOWN) {
            movement.x -= moveAmount * deltaTime;
        }
        if (params.state & params.A_DOWN) {
            movement.y -= moveAmount * deltaTime;
        } else if (params.state & params.D_DOWN) {
            movement.y += moveAmount * deltaTime;
        }

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        //Set up MVP
        glm::mat4 model(1.0f);
        //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::translate(view, movement);
        glm::mat4 mvp = proj * view * model;

        glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        GLint uniMvp = glGetUniformLocation(program, "mvp");
        glUniformMatrix4fv(uniMvp, 1, GL_FALSE, glm::value_ptr(mvp));

        glfwSwapBuffers(window);

        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR) {
            std::cout << err << std::endl;
        }
	}

	glfwDestroyWindow(window);
	glfwTerminate();

}


