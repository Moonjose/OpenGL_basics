#include <glad/glad.h>  
#include <GLFW/glfw3.h> 
#include <iostream>

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
unsigned int compileShader(GLenum type, const char* source);
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "}\0";

const char* fragmentShaderSourceOrange =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

const char* fragmentShaderSourceYellow =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\n\0";

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

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragOrange = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSourceOrange);
    unsigned int fragYellow = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSourceYellow);

    unsigned int shaderOrange = createProgram(vertexShader, fragOrange);
    unsigned int shaderYellow = createProgram(vertexShader, fragYellow);

    glDeleteShader(vertexShader);
    glDeleteShader(fragOrange);
    glDeleteShader(fragYellow);

    PrimitiveBuffers buffers = setupPrimitive(Triangle);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (buffers.useEBO) {
            glUseProgram(shaderOrange);
            glBindVertexArray(buffers.VAO[0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else {
            glUseProgram(shaderOrange);
            glBindVertexArray(buffers.VAO[0]);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glUseProgram(shaderYellow);
            glBindVertexArray(buffers.VAO[1]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, buffers.VAO);
    glDeleteBuffers(2, buffers.VBO);
    if (buffers.useEBO) glDeleteBuffers(1, &buffers.EBO);
    glDeleteProgram(shaderYellow);
    glDeleteProgram(shaderOrange);
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

unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Erro ao linkar programa:\n" << infoLog << std::endl;
    }

    return program;
}

unsigned int compileShader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Erro ao compilar shader:\n" << infoLog << std::endl;
    }

    return shader;
}

PrimitiveBuffers setupPrimitive(PrimitiveShape shape) {
    PrimitiveBuffers buffers = {};
    buffers.useEBO = false;

    float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
        -0.45f, 0.5f, 0.0f
    };

    float secondTriangle[] = {
         0.0f, -0.5f, 0.0f,
         0.9f, -0.5f, 0.0f,
         0.45f, 0.5f, 0.0f
    };

    float rectangleVertices[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    unsigned int rectangleIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(2, buffers.VAO);
    glGenBuffers(2, buffers.VBO);

    auto setupVAO = [](unsigned int vao, unsigned int vbo, const float* data, size_t size) {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    };

    if (shape == Triangle) {
        setupVAO(buffers.VAO[0], buffers.VBO[0], firstTriangle, sizeof(firstTriangle));
        setupVAO(buffers.VAO[1], buffers.VBO[1], secondTriangle, sizeof(secondTriangle));
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
