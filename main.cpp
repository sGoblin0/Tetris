#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 1920; // escolham as vossa resoluçao de ecra !!!!!!!!!!!!!
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(5.5f, 11.0f, 40.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// cube position
glm::vec3 cubePos(0.0f, 0.0f, 0.0f);

glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);

float speed = 0.03f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tetris 3D", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader basicShader("shaders/basic_vertex.vs", "shaders/basic_fragment.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = { // CUBE
        //posiçao (x,y,z)        texture box
        -0.5f, -0.5f, -0.5f,     1.0f, 0.0f, // -z
         0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,     0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,     0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,     1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     1.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,     0.0f, 0.0f, // +z
         0.5f, -0.5f,  0.5f,     1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,     0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,     0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,// -x
        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,     1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,

         0.5f,  0.5f,  0.5f,     0.0f, 1.0f, // +x
         0.5f,  0.5f, -0.5f,     1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,     1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,     1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f, // -y
         0.5f, -0.5f, -0.5f,     1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,     1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,     0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f, // +y
         0.5f,  0.5f, -0.5f,     1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,     1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,     1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,     0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,     0.0f, 1.0f
    };
    glm::vec3 cubePositions[] = { // POSITIONS OF CUBES IN FRAME
        glm::vec3(0.0f,  0.0f,  0.0f), // base
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  0.0f,  0.0f),
        glm::vec3(3.0f,  0.0f,  0.0f),
        glm::vec3(4.0f,  0.0f,  0.0f),
        glm::vec3(5.0f,  0.0f,  0.0f),
        glm::vec3(6.0f,  0.0f,  0.0f),
        glm::vec3(7.0f,  0.0f,  0.0f),
        glm::vec3(8.0f,  0.0f,  0.0f),
        glm::vec3(9.0f,  0.0f,  0.0f),
        glm::vec3(10.0f,  0.0f,  0.0f),
        glm::vec3(11.0f,  0.0f,  0.0f),

        glm::vec3(11.0f,  1.0f,  0.0f), // coluna direita
        glm::vec3(11.0f,  2.0f,  0.0f),
        glm::vec3(11.0f,  3.0f,  0.0f),
        glm::vec3(11.0f,  4.0f,  0.0f),
        glm::vec3(11.0f,  5.0f,  0.0f),
        glm::vec3(11.0f,  6.0f,  0.0f),
        glm::vec3(11.0f,  7.0f,  0.0f),
        glm::vec3(11.0f,  8.0f,  0.0f),
        glm::vec3(11.0f,  9.0f,  0.0f),
        glm::vec3(11.0f,  10.0f,  0.0f),
        glm::vec3(11.0f,  11.0f,  0.0f),
        glm::vec3(11.0f,  12.0f,  0.0f),
        glm::vec3(11.0f,  13.0f,  0.0f),
        glm::vec3(11.0f,  14.0f,  0.0f),
        glm::vec3(11.0f,  15.0f,  0.0f),
        glm::vec3(11.0f,  16.0f,  0.0f),
        glm::vec3(11.0f,  17.0f,  0.0f),
        glm::vec3(11.0f,  18.0f,  0.0f),
        glm::vec3(11.0f,  19.0f,  0.0f),
        glm::vec3(11.0f,  20.0f,  0.0f),
        glm::vec3(11.0f,  21.0f,  0.0f),

        glm::vec3(11.0f,  22.0f,  0.0f), // top
        glm::vec3(10.0f,  22.0f,  0.0f),
        glm::vec3(9.0f,  22.0f,  0.0f),
        glm::vec3(8.0f,  22.0f,  0.0f),
        glm::vec3(7.0f,  22.0f,  0.0f),
        glm::vec3(6.0f,  22.0f,  0.0f),
        glm::vec3(5.0f,  22.0f,  0.0f),
        glm::vec3(4.0f,  22.0f,  0.0f),
        glm::vec3(3.0f,  22.0f,  0.0f),
        glm::vec3(2.0f,  22.0f,  0.0f),
        glm::vec3(1.0f,  22.0f,  0.0f),
        glm::vec3(0.0f,  22.0f,  0.0f),

        glm::vec3(0.0f,  21.0f,  0.0f),
        glm::vec3(0.0f,  20.0f,  0.0f),
        glm::vec3(0.0f,  19.0f,  0.0f),
        glm::vec3(0.0f,  18.0f,  0.0f), // coluna esquerda
        glm::vec3(0.0f,  17.0f,  0.0f),
        glm::vec3(0.0f,  16.0f,  0.0f),
        glm::vec3(0.0f,  15.0f,  0.0f),
        glm::vec3(0.0f,  14.0f,  0.0f),
        glm::vec3(0.0f,  13.0f,  0.0f),
        glm::vec3(0.0f,  12.0f,  0.0f),
        glm::vec3(0.0f,  11.0f,  0.0f),
        glm::vec3(0.0f,  10.0f,  0.0f),
        glm::vec3(0.0f,  9.0f,  0.0f),
        glm::vec3(0.0f,  8.0f,  0.0f),
        glm::vec3(0.0f,  7.0f,  0.0f),
        glm::vec3(0.0f,  6.0f,  0.0f),
        glm::vec3(0.0f,  5.0f,  0.0f),
        glm::vec3(0.0f,  4.0f,  0.0f),
        glm::vec3(0.0f,  3.0f,  0.0f),
        glm::vec3(0.0f,  2.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
    };
    glm::vec3 cubeColors[] = { //COLORS FOR THE FRAME
        glm::vec3(1.0f,  0.0f,  0.0f), // base
        glm::vec3(1.0f,  0.0909f,  0.0f),
        glm::vec3(1.0f,  0.1818f,  0.0f),
        glm::vec3(1.0f,  0.2727f,  0.0f),
        glm::vec3(1.0f,  0.3636f,  0.0f),
        glm::vec3(1.0f,  0.4545f,  0.0f),
        glm::vec3(1.0f,  0.5454f,  0.0f),
        glm::vec3(1.0f,  0.6363f,  0.0f),
        glm::vec3(1.0f,  0.7272f,  0.0f),
        glm::vec3(1.0f,  0.8181f,  0.0f),
        glm::vec3(1.0f,  0.9090f,  0.0f),
        glm::vec3(1.0f,  1.0f,  0.0f),

        glm::vec3(0.9090f,  1.0f,  0.0f), // coluna direita
        glm::vec3(0.8181f,  1.0f,  0.0f),
        glm::vec3(0.7272f,  1.0f,  0.0f),
        glm::vec3(0.6363f,  1.0f,  0.0f),
        glm::vec3(0.5454f,  1.0f,  0.0f),
        glm::vec3(0.4545f,  1.0f,  0.0f),
        glm::vec3(0.3636f,  1.0f,  0.0f),
        glm::vec3(0.2727f,  1.0f,  0.0f),
        glm::vec3(0.1818f,  1.0f,  0.0f),
        glm::vec3(0.0909f,  1.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0909f),
        glm::vec3(0.0f,  1.0f,  0.1818f),
        glm::vec3(0.0f,  1.0f,  0.2717f),
        glm::vec3(0.0f,  1.0f,  0.3636f),
        glm::vec3(0.0f,  1.0f,  0.4545f),
        glm::vec3(0.0f,  1.0f,  0.5454f),
        glm::vec3(0.0f,  1.0f,  0.6363f),
        glm::vec3(0.0f,  1.0f,  0.7272f),
        glm::vec3(0.0f,  1.0f,  0.8181f),
        glm::vec3(0.0f,  1.0f,  0.9090f),
        
        glm::vec3(0.0f,  1.0f,  1.0f), // top
        glm::vec3(0.0f,  0.9090f,  1.0f),
        glm::vec3(0.0f,  0.8181f,  1.0f),
        glm::vec3(0.0f,  0.7272f,  1.0f),
        glm::vec3(0.0f,  0.6363f,  1.0f),
        glm::vec3(0.0f,  0.5454f,  1.0f),
        glm::vec3(0.0f,  0.4545f,  1.0f),
        glm::vec3(0.0f,  0.3636f,  1.0f),
        glm::vec3(0.0f,  0.2727f,  1.0f),
        glm::vec3(0.0f,  0.1818f,  1.0f),
        glm::vec3(0.0f,  0.0909f,  1.0f),
        glm::vec3(0.0f,  0.0f,  1.0f),

        glm::vec3(0.0909f,  0.0f,  1.0f), // coluna esquerda
        glm::vec3(0.1818f,  0.0f,  1.0f),
        glm::vec3(0.2727f,  0.0f,  1.0f),
        glm::vec3(0.3636f,  0.0f,  1.0f),
        glm::vec3(0.4545f,  0.0f,  1.0f),
        glm::vec3(0.5454f,  0.0f,  1.0f),
        glm::vec3(0.6363f,  0.0f,  1.0f),
        glm::vec3(0.7272f,  0.0f,  1.0f),
        glm::vec3(0.8181f,  0.0f,  1.0f),
        glm::vec3(0.9090f,  0.0f,  1.0f),
        glm::vec3(1.0f,  0.0f,  1.0f),
        glm::vec3(1.0f,  0.0f,  0.9090f),
        glm::vec3(1.0f,  0.0f,  0.8181f),
        glm::vec3(1.0f,  0.0f,  0.7272f),
        glm::vec3(1.0f,  0.0f,  0.6363f),
        glm::vec3(1.0f,  0.0f,  0.5454f),
        glm::vec3(1.0f,  0.0f,  0.4545f),
        glm::vec3(1.0f,  0.0f,  0.3636f),
        glm::vec3(1.0f,  0.0f,  0.2727f),
        glm::vec3(1.0f,  0.0f,  0.1818f),
        glm::vec3(1.0f,  0.0f,  0.0909f),
    };
    glm::vec3 pieceL[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  2.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),
    };
    glm::vec3 pieceI[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  2.0f,  0.0f),
        glm::vec3(0.0f,  3.0f,  0.0f),
    };
    glm::vec3 pieceSquare[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  1.0f,  0.0f),
    };
    glm::vec3 pieceLopposite[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  2.0f,  0.0f),
        glm::vec3(1.0f,  2.0f,  0.0f),
    };
    glm::vec3 pieceZ[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(1.0f,  1.0f,  0.0f),
        glm::vec3(1.0f,  2.0f,  0.0f),
    };
    glm::vec3 pieceZopposite[] = {
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  2.0f,  0.0f),
    };
    glm::vec3 pieceDick[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  2.0f,  0.0f),
        glm::vec3(1.0f,  1.0f,  0.0f),
    };

    //cout << "Pos: " << size(cubePositions) << endl;
    //cout << "Cor: " << size(cubeColors) << endl;

    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);
    // vertices position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture = loadTexture("textures/blockBW.png");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        bool R = false, G = true, B = false;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        //-----------------------------------------cube-------------------------------------------------------
        basicShader.use();
        //basicShader.setVec3("color", cubeColor);
        basicShader.setMat4("projection", projection);
        basicShader.setMat4("view", view);

        for (unsigned int i = 0; i < size(cubePositions); i++) { // draw square frame
            basicShader.setVec3("color", cubeColors[i]);
            model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            basicShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (unsigned int i = 0; i < size(pieceL); i++) { // draw L piece
            basicShader.setVec3("color", 0.4196f, 0.55686f, 0.13725f);
            model = glm::translate(glm::mat4(1.0f), pieceL[i]);
            model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
            basicShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (unsigned int i = 0; i < size(pieceLopposite); i++) { // draw L opposite piece
            basicShader.setVec3("color", 0.941176f, 0.50196f, 0.50196f);
            model = glm::translate(glm::mat4(1.0f), pieceLopposite[i]);
            model = glm::translate(model, glm::vec3(-3.0f, 4.0f, 0.0f));
            basicShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        for (unsigned int i = 0; i < size(pieceI); i++) { // draw I piece
            basicShader.setVec3("color", 0.50196f, 0.50196f, 0.0f);
            model = glm::translate(glm::mat4(1.0f), pieceI[i]);
            model = glm::translate(model, glm::vec3(-3.0f, 8.0f, 0.0f));
            basicShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (unsigned int i = 0; i < size(pieceSquare); i++) { // draw Square piece
            basicShader.setVec3("color", 0.54509f, 0.27058f, 0.074509f);
            model = glm::translate(glm::mat4(1.0f), pieceSquare[i]);
            model = glm::translate(model, glm::vec3(-3.0f, 13.0f, 0.0f));
            basicShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (unsigned int i = 0; i < size(pieceZ); i++) { // draw Z piece
            basicShader.setVec3("color", 0.4f, 0.80392f, 0.66666f);
            model = glm::translate(glm::mat4(1.0f), pieceZ[i]);
            model = glm::translate(model, glm::vec3(-6.0f, 0.0f, 0.0f));
            basicShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (unsigned int i = 0; i < size(pieceZopposite); i++) { // draw Z opposite piece
            basicShader.setVec3("color", 0.372549f, 0.6196f, 0.62745f);
            model = glm::translate(glm::mat4(1.0f), pieceZopposite[i]);
            model = glm::translate(model, glm::vec3(-6.0f, 4.0f, 0.0f));
            basicShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (unsigned int i = 0; i < size(pieceDick); i++) { // draw Dick piece
            basicShader.setVec3("color", 0.48235f, 0.40784f, 0.93333f);
            model = glm::translate(glm::mat4(1.0f), pieceDick[i]);
            model = glm::translate(model, glm::vec3(-6.0f, 8.0f, 0.0f));
            basicShader.setMat4("model", model);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(UPWARD, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(DOWNWARD, deltaTime + 0.05);

    //if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) cubePos.z -= speed;
    //if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) cubePos.z += speed;
    //if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) cubePos.x -= speed;
    //if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) cubePos.x += speed;
    //if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS) cubePos.y -= speed;
    //if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) cubePos.y += speed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// ---------------------------------------------------------------------------------------------------------
// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ---------------------------------------------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
