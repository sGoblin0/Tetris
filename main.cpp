#include <glad\glad.h>

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>

#include <iostream>
#include <list>

#include "piecesCoord.h"

using namespace std;
using namespace glm;

float M_PI = 3.14159265358979323846;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

void wait();

// settings
const unsigned int SCR_WIDTH = 1920; // escolham as vossa resoluçao de ecra !!!!!!!!!!!!!
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(vec3(5.5f, 11.0f, 30.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


//MVP
mat4 projection = mat4(1.0f);
mat4 view = mat4(1.0f);
mat4 model = mat4(1.0f);

// helpful variables
float speed = 1.0f;
//by comparing the next two variables, we can check if the piece number has changed
int option; 
int currentOption; 


//piece spawn position
vec3 pieceSpawnPosition(4.0f, 20.0f, 0.0f);




typedef struct PIECE_BOARD {
    vec2 pos; //position of the piece in the board
	int index; //index of the piece (option)
    vec3 coord[4]; //coordenates (position of cubes in relation the the center cube)
}PIECE_BOARD;

vector<PIECE_BOARD> allPiecesInMap; //10 x 22 (regras jogo)




typedef struct PIECE{
    vec3 position = vec3(4.0f, 20.0f, 0.0f); // current cube position
    vec3 futurePosition = vec3(4.0f, 20.0f, 0.0f); //future cube position (after movement)
    vec3 coord[4]; //coordenates (position of cubes in relation the the center cube)
    int rot = 0; //rotation
}PIECE;
PIECE tmpPiece; //current piece




//checks if any cube is going to collide with another (wall or pieces)
bool checkColision(bool rotation = false) {
    if (tmpPiece.position == tmpPiece.futurePosition && !rotation) return false; //no need to check for collision
    for (int n = 0; n < 4; n++) { //for every cube of the piece
        vec2 sum = vec2(tmpPiece.coord[n].x + tmpPiece.futurePosition.x, tmpPiece.coord[n].y + tmpPiece.futurePosition.y); //cube position after transladation
        for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++) { //for every cube of the walls 
            if (sum.x == cubePositions[i].x && sum.y == cubePositions[i].y) {
                tmpPiece.futurePosition = tmpPiece.position; //colision, future location disregarded
                return true;
            }
        }
        for (unsigned int i = 0; i < allPiecesInMap.size(); i++) { //for every piece already on the board
            for (unsigned int m = 0; m < 4; m++) { //for every cube of every piece already on the board
                vec3 sumCubes = vec3(allPiecesInMap.at(i).pos, 0.0f) + allPiecesInMap.at(i).coord[m];
                if (sum.x == sumCubes.x && sum.y == sumCubes.y) {
                    tmpPiece.futurePosition = tmpPiece.position;
                    return true;
                }
            }
        }
    }
    tmpPiece.position = tmpPiece.futurePosition; //no colision, piece moves for future location
    return false;
}

//----------------------------------------offsets-----------------------------------------

vec3 offset01[] = {  //also works for 2>>1
    vec3(-1.0f, 0.0f, 0.0f),
    vec3(-1.0f, 1.0f, 0.0f),
    vec3(0.0f, -2.0f, 0.0f),
    vec3(-1.0f, -2.0f, 0.0f)
};

vec3 offset10[] = {  //also works for 1>>2
    vec3(1.0f, 0.0f, 0.0f),
    vec3(1.0f, -1.0f, 0.0f),
    vec3(0.0f, 2.0f, 0.0f),
    vec3(1.0f, 2.0f, 0.0f),
};


vec3 offset23[] = { //also works for 0 -> 3
    vec3(1.0f, 0.0f, 0.0f),
    vec3(1.0f, 1.0f, 0.0f),
    vec3(0.0f, -2.0f, 0.0f),
    vec3(1.0f, -2.0f, 0.0f)
};

vec3 offset32[] = {
    vec3(-1.0f, 0.0f, 0.0f),
    vec3(-1.0f, -1.0f, 0.0f),
    vec3(0.0f, 2.0f, 0.0f),
    vec3(-1.0f, -2.0f, 0.0f)
};



vec3 offsetNull[] = {
    vec3(0.0f, 0.0f, 0.0f),
    vec3(0.0f, 0.0f, 0.0f),
    vec3(0.0f, 0.0f, 0.0f),
    vec3(0.0f, 0.0f, 0.0f)
};

//check table for details
vec3 offsetMatrix[8][4] = {
    *offsetNull,* offset01,* offsetNull,* offset23,
    *offset10, *offsetNull, *offset10, *offsetNull,
    *offsetNull, *offset01, *offsetNull, *offset23,
    *offset32, *offsetNull, *offset32, *offsetNull 
};


//-----------------------------------------------------------------------------------------


//rotation from/to.
//f.e rotation from spawn to clockwise: {0,1}
//f.e roation from spawn to conterclockwise: {0,3}
int rotation[] = { 0,0 };  

//to rotate the pieces, and apply the offsets if necessary
void rotationFunc(bool dir) { //dir == true -> counter clockwise
    int x = 0;
    rotation[0] = rotation[1];
    if (dir) {
        x = -1;
        rotation[1]--;
        if (rotation[1] < 0) rotation[1] = 3;	
    }
    else {
        x = 1;
        rotation[1]++;
        if (rotation[1] > 3) rotation[1] = 0;
    }
    

    //rotate all pieces 
    for (int i = 0; i < 4; i++) {
        mat4 tmpm = mat4(1.0f);
        tmpm = rotate(tmpm, radians(x * 90.0f), vec3(0.0f, 0.0f, 1.0f));
        tmpm = translate(tmpm, tmpPiece.coord[i]);
        tmpPiece.coord[i] = vec4(tmpPiece.coord[i], 1.0f) * tmpm;
        for (int i = 0; i < 4; i++) {
            tmpPiece.coord[i].x = round(tmpPiece.coord[i].x);
            tmpPiece.coord[i].y = round(tmpPiece.coord[i].y);
        }
        
    }



    //offsets
    int offset_n = 0; //offset number | after 5 tries, doesn't rotate
    while (true) {
        if (offset_n > 3) {
            offset_n = 0;
            break; //exit loop       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
        if (checkColision(true)) {
            printf("It indeed has colition!!!!\n\n");
            tmpPiece.futurePosition = tmpPiece.futurePosition + offsetMatrix[rotation[0], rotation[1]][offset_n]; //tries with offset
            offset_n++;
            continue;
        }
        //there was no colision or the offset worked
        offset_n = 0;
        tmpPiece.position = tmpPiece.futurePosition;
        break; //exit loop
    }
}





void randomPiece() {
    
    option =  (rand() % 7); //choose random piece (0-6)
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//register the piece on the board
void registerPiece() {
    PIECE_BOARD tmp;
    tmp.pos = tmpPiece.position;
    tmp.index = option;
    for (unsigned int i = 0; i < 4; i++) {
        tmp.coord[i] = tmpPiece.coord[i];
    }
    
	allPiecesInMap.push_back(tmp);
    randomPiece();
}


void drawPiece() {

}

int main()
{

    srand(time(0));
    
    
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
    //GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tetris 3D", glfwGetPrimaryMonitor(), NULL);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tetris 3D", NULL, NULL);
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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader basicShader("shaders/pieces.vs", "shaders/pieces.fs");

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
    
    vec3 cubeColors[] = { //COLORS FOR THE FRAME
        vec3(1.0f,  0.0f,  0.0f), // base
        vec3(1.0f,  0.0909f,  0.0f),
        vec3(1.0f,  0.1818f,  0.0f),
        vec3(1.0f,  0.2727f,  0.0f),
        vec3(1.0f,  0.3636f,  0.0f),
        vec3(1.0f,  0.4545f,  0.0f),
        vec3(1.0f,  0.5454f,  0.0f),
        vec3(1.0f,  0.6363f,  0.0f),
        vec3(1.0f,  0.7272f,  0.0f),
        vec3(1.0f,  0.8181f,  0.0f),
        vec3(1.0f,  0.9090f,  0.0f),
        vec3(1.0f,  1.0f,  0.0f),

        vec3(0.9090f,  1.0f,  0.0f), // coluna direita
        vec3(0.8181f,  1.0f,  0.0f),
        vec3(0.7272f,  1.0f,  0.0f),
        vec3(0.6363f,  1.0f,  0.0f),
        vec3(0.5454f,  1.0f,  0.0f),
        vec3(0.4545f,  1.0f,  0.0f),
        vec3(0.3636f,  1.0f,  0.0f),
        vec3(0.2727f,  1.0f,  0.0f),
        vec3(0.1818f,  1.0f,  0.0f),
        vec3(0.0909f,  1.0f,  0.0f),
        vec3(0.0f,  1.0f,  0.0f),
        vec3(0.0f,  1.0f,  0.0909f),
        vec3(0.0f,  1.0f,  0.1818f),
        vec3(0.0f,  1.0f,  0.2717f),
        vec3(0.0f,  1.0f,  0.3636f),
        vec3(0.0f,  1.0f,  0.4545f),
        vec3(0.0f,  1.0f,  0.5454f),
        vec3(0.0f,  1.0f,  0.6363f),
        vec3(0.0f,  1.0f,  0.7272f),
        vec3(0.0f,  1.0f,  0.8181f),
        vec3(0.0f,  1.0f,  0.9090f),

        vec3(0.0f,  1.0f,  1.0f), // top
        vec3(0.0f,  0.9090f,  1.0f),
        vec3(0.0f,  0.8181f,  1.0f),
        vec3(0.0f,  0.7272f,  1.0f),
        vec3(0.0f,  0.6363f,  1.0f),
        vec3(0.0f,  0.5454f,  1.0f),
        vec3(0.0f,  0.4545f,  1.0f),
        vec3(0.0f,  0.3636f,  1.0f),
        vec3(0.0f,  0.2727f,  1.0f),
        vec3(0.0f,  0.1818f,  1.0f),
        vec3(0.0f,  0.0909f,  1.0f),
        vec3(0.0f,  0.0f,  1.0f),

        vec3(0.0909f,  0.0f,  1.0f), // coluna esquerda
        vec3(0.1818f,  0.0f,  1.0f),
        vec3(0.2727f,  0.0f,  1.0f),
        vec3(0.3636f,  0.0f,  1.0f),
        vec3(0.4545f,  0.0f,  1.0f),
        vec3(0.5454f,  0.0f,  1.0f),
        vec3(0.6363f,  0.0f,  1.0f),
        vec3(0.7272f,  0.0f,  1.0f),
        vec3(0.8181f,  0.0f,  1.0f),
        vec3(0.9090f,  0.0f,  1.0f),
        vec3(1.0f,  0.0f,  1.0f),
        vec3(1.0f,  0.0f,  0.9090f),
        vec3(1.0f,  0.0f,  0.8181f),
        vec3(1.0f,  0.0f,  0.7272f),
        vec3(1.0f,  0.0f,  0.6363f),
        vec3(1.0f,  0.0f,  0.5454f),
        vec3(1.0f,  0.0f,  0.4545f),
        vec3(1.0f,  0.0f,  0.3636f),
        vec3(1.0f,  0.0f,  0.2727f),
        vec3(1.0f,  0.0f,  0.1818f),
        vec3(1.0f,  0.0f,  0.0909f),
    };
    


    vec3 allPiecesColors[] = {
        vec3(0.4196f, 0.55686f, 0.13725f),
        vec3(0.941176f, 0.50196f, 0.50196f),
        vec3(0.50196f, 0.50196f, 0.0f),
        vec3(0.54509f, 0.27058f, 0.074509f),
        vec3(0.4f, 0.80392f, 0.66666f),
        vec3(0.372549f, 0.6196f, 0.62745f),
        vec3(0.48235f, 0.40784f, 0.93333f)
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

    currentOption = -1;
    randomPiece();

  
    


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        bool R = false, G = true, B = false; //?????????????

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        //-----------------------------------------frame-------------------------------------------------------
        basicShader.use();
        basicShader.setMat4("projection", projection);
        basicShader.setMat4("view", view);


        //check if future position is valid (if creates collision)
        checkColision();
      

        for (unsigned int i = 0; i < size(cubePositions); i++) { // draw square frame
            basicShader.setVec3("color", cubeColors[i]);
            model = translate(mat4(1.0f), cubePositions[i]);
            basicShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }






        //-----------------------------------------pieces-------------------------------------------------------
        if (currentOption != option) { //new piece
            for (int i = 0; i < 4; i++) {
                printf("asdasd\n");
                tmpPiece.coord[i] = allPieces[option][i];
            }
            tmpPiece.position = pieceSpawnPosition;
            currentOption = option;
        }
        for (unsigned int i = 0; i < 4; i++){ // draw current piece
			basicShader.setVec3("color", allPiecesColors[option][0], allPiecesColors[option][1], allPiecesColors[option][2]);
            

            model = mat4(1.0f);
            model = translate(model, tmpPiece.position); //current position of the center piece
            model = translate(model, tmpPiece.coord[i]);//mount pieces together
          
            basicShader.setMat4("model", model);
            //printf("%d\n", i);
            //printf("%f %f %f %f\n\n", model[0][0], model[0][1], model[1][0], model[1][1]);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);

            
        }

        //-----------------------------------------previous pieces-------------------------------------------------------
        for (unsigned int i = 0; i < allPiecesInMap.size(); i++) { //draw previous pieces    
            int index = allPiecesInMap.at(i).index; //piece number
            vec3 pos = vec3(allPiecesInMap.at(i).pos, 0.0f); //coordenate of the piece
            vec3* coord = allPiecesInMap.at(i).coord; //coordenates of the cubes in the piece
            for (unsigned int n = 0; n < 4; n++) {
                basicShader.setVec3("color", allPiecesColors[index][0], allPiecesColors[index][1], allPiecesColors[index][2]);
                model = mat4(1.0f);
                model = translate(model, pos); //current position of the center piece
                model = translate(model, coord[n]);//mount pieces together

                basicShader.setMat4("model", model);
                //printf("%d\n", i);
                //printf("%f %f %f %f\n\n", model[0][0], model[0][1], model[1][0], model[1][1]);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            
            
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


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void debugFunction() {
    printf("-------------------------------\n");
    printf("Current relative position: \n");
    for (unsigned int i = 0; i < 4; i++) {
        printf("%f %f \n", tmpPiece.coord[i].x, tmpPiece.coord[i].y );

    }

    printf("Current position in map: \n");
    for (unsigned int i = 0; i < 4; i++) {
        printf("%f %f \n", tmpPiece.coord[i].x + tmpPiece.position.x, tmpPiece.coord[i].y + tmpPiece.position.y);

    }
    printf("----------------------------------\n");

    for (unsigned int i = 0; i < 4; i++) {
        if (tmpPiece.coord[i].x == -1.0f && tmpPiece.coord[i].y == -0.0f)
            printf("Found it!!\n");
        

    }
    for (unsigned int i = 0; i < 4; i++)
        printf("type: %s\n", typeid(tmpPiece.coord[i].x).name())
        ;
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

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { tmpPiece.futurePosition.x -= speed; wait(); }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { tmpPiece.futurePosition.x += speed; wait(); }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { tmpPiece.futurePosition.y -= speed; wait(); }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { tmpPiece.futurePosition.y += speed; wait(); }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) { 
        randomPiece(); wait();
		
    }
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && option != 2) { //clockwise (option 2 == cube, ergo no rotation)
        rotationFunc(false);
        wait(); 
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && option != 2) { //counter clockwise
        rotationFunc(true);
        wait(); 
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) debugFunction();

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { registerPiece(); wait(); }
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



void wait() {
    glfwWaitEventsTimeout(0.7);
}