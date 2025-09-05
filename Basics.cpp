#include <glad/glad.h>  
#include <GLFW/glfw3.h> 
#include <iostream>

// ---------------------------------------------------------
// Fun��es utilit�rias
// ---------------------------------------------------------

enum PrimitiveShape
{
    Triangle,
    Rectangle
};

struct PrimitiveBuffers {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO; // 0 se n�o for usado
    bool useEBO;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
PrimitiveBuffers setupPrimitive(PrimitiveShape shape);

// ---------------------------------------------------------
// Configura��es da janela
// ---------------------------------------------------------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ---------------------------------------------------------
// C�digo-fonte do Vertex Shader (processa posi��es dos v�rtices)
// ---------------------------------------------------------
const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"  // Entrada: posi��o do v�rtice (3 floats)
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"    // Converte vec3 -> vec4 para sa�da
    "}\0";

// ---------------------------------------------------------
// C�digo-fonte do Fragment Shader (define cor dos pixels)
// ---------------------------------------------------------
const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"                  // Sa�da: cor final do fragmento
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"  // Cor laranja
    "}\n\0";

// ---------------------------------------------------------
// Fun��o principal
// ---------------------------------------------------------
int main()
{
    // -----------------------------------------------------
    // Inicializa��o do GLFW e cria��o da janela
    // -----------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Usar OpenGL 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Necess�rio no macOS
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Erro: Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // -----------------------------------------------------
    // Inicializa��o do GLAD (carrega fun��es do OpenGL)
    // -----------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Erro: Falha ao inicializar o GLAD" << std::endl;
        return -1;
    }

    // -----------------------------------------------------
    // Compila��o e linkagem dos Shaders
    // -----------------------------------------------------
    // Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Verifica��o de erros no Vertex Shader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERRO::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Verifica��o de erros no Fragment Shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERRO::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Criar programa de Shader e linkar os dois
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Verifica��o de erros no link do programa
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERRO::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Limpamos shaders j� linkados
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // Configura buffers de acordo com o primitivo escolhido
    PrimitiveShape currentShape = Triangle; 
    PrimitiveBuffers buffers = setupPrimitive(currentShape);

    // -----------------------------------------------------
    // Loop de renderiza��o
    // -----------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Entrada do usu�rio
        processInput(window);

        // Limpar a tela com uma cor
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenhar tri�ngulo (ou ret�ngulo se usar o EBO e 4 v�rtices)
        glUseProgram(shaderProgram);
        glBindVertexArray(buffers.VAO); // Obt�m os dados de v�rtices e como desenh�-los

        if (buffers.useEBO) {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
       
        // Troca de buffers e eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // -----------------------------------------------------
    // Limpeza de recursos
    // -----------------------------------------------------
    glDeleteVertexArrays(1, &buffers.VAO);
    glDeleteBuffers(1, &buffers.VBO);
    if (buffers.useEBO) glDeleteBuffers(1, &buffers.EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

// ---------------------------------------------------------
// Fun��es auxiliares
// ---------------------------------------------------------
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

    float triangleVertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    float rectangleVertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };

    unsigned int rectangleIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &buffers.VAO); // Cria VAO
    glGenBuffers(1, &buffers.VBO); // Cria VBO

    glBindVertexArray(buffers.VAO); // Bind do VAO (sempre primeiro, memoriza o que vem depois)
    glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO); // Bind do VBO

    // Envia os dados para o buffer dependendo do shape escolhido
    if (shape == Triangle)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); 
    }
    else if (shape == Rectangle) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

        // Gera e envia EBO para o buffer (reaproveitar v�rtices com �ndices EBO)
        buffers.useEBO = true;
        glGenBuffers(1, &buffers.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);
    }

    // Define como o atributo ser� lido pelo Vertex Shader (layout location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind (boa pr�tica, opcional)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return buffers;
}
