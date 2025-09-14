#include <glad/glad.h>  
#include <GLFW/glfw3.h> 
#include <iostream>
#include "Shader.h"

enum PrimitiveShape { Triangle, Rectangle };

struct PrimitiveBuffers {
    unsigned int VAO[2];
    unsigned int VBO[2];
    unsigned int EBO;
    bool useEBO;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
PrimitiveBuffers setupPrimitive(PrimitiveShape shape);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (!window) {
        std::cout << "Erro ao criar a janela GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Erro ao inicializar o GLAD\n";
        return -1;
    }

    Shader ourShader("3.3.shader.vs", "3.3.shader.fs");

    PrimitiveBuffers buffers = setupPrimitive(Triangle);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (buffers.useEBO) {
            ourShader.use();
            glBindVertexArray(buffers.VAO[0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else {
            ourShader.use();;
            glBindVertexArray(buffers.VAO[0]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, buffers.VAO);
    glDeleteBuffers(2, buffers.VBO);
    if (buffers.useEBO) glDeleteBuffers(1, &buffers.EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

PrimitiveBuffers setupPrimitive(PrimitiveShape shape) {
    PrimitiveBuffers buffers = {};
    buffers.useEBO = false;

    float firstTriangle[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };

    //float secondTriangle[] = {
    //     0.0f, -0.5f, 0.0f,
    //     0.9f, -0.5f, 0.0f,
    //     0.45f, 0.5f, 0.0f
    //};

    float rectangleVertices[] = {
        // positions         // colors
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // top right (red)
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom right (green)
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // bottom left (blue)
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f   // top left (yellow)
    };

    unsigned int rectangleIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(2, buffers.VAO);
    glGenBuffers(2, buffers.VBO);

    auto setupVAO = [shape](unsigned int vao, unsigned int vbo, const float* data, size_t size) {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    };

    if (shape == Triangle) {
        setupVAO(buffers.VAO[0], buffers.VBO[0], firstTriangle, sizeof(firstTriangle));
        //setupVAO(buffers.VAO[1], buffers.VBO[1], secondTriangle, sizeof(secondTriangle));
    }
    else {
        setupVAO(buffers.VAO[0], buffers.VBO[0], rectangleVertices, sizeof(rectangleVertices));

        buffers.useEBO = true;
        glGenBuffers(1, &buffers.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);
    }

    return buffers;
}
