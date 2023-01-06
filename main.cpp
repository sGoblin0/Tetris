#include <glad\glad.h>

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>

#include <iostream>
#include <list>
#include <thread>         // thread
#include <map>
#include <string>
#include <chrono>
#include <mutex>
#include <fstream>

#include "piecesCoord.h"

using namespace std;
using namespace glm;

float M_PI = 3.14159265358979323846;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void RenderText(Shader& shader, string text, float x, float y, float scale, glm::vec3 color);

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
int points = 0;

//by comparing the next two variables, we can check if the piece number has changed
int option = (rand() % 7);
int nextOption = (rand() % 7);

//piece spawn position
vec3 pieceSpawnPosition(4.0f,20.0f, 0.0f);


/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};
map<GLchar, Character> Characters;
unsigned int textVAO, textVBO;



typedef struct PIECE{
    vec3 position = vec3(4.0f, 20.0f, 0.0f); // current cube position
    vec3 futurePosition = vec3(4.0f, 20.0f, 0.0f); //future cube position (after movement)
    vec3 coord[4]; //coordenates (position of cubes in relation the the center cube)
    int rot = 0; //rotation
}PIECE;
PIECE tmpPiece; //current piece


//every position correspondes to the index of the color of the cube
//-1 = empty
int allPiecesInBoard[21][10]; 



//checks if any cube is going to collide with another (wall or pieces)
bool checkColision(bool rotation = false) {
    if (tmpPiece.position == tmpPiece.futurePosition && !rotation) return false; //no need to check for collision
    for (int n = 0; n < 4; n++) { //for every cube of the piece
        vec2 sum = vec2(tmpPiece.coord[n].x + tmpPiece.futurePosition.x, tmpPiece.coord[n].y + tmpPiece.futurePosition.y); //cube position after transladation
        
        if (allPiecesInBoard[int(sum.y) - 1][int(sum.x) - 1] != -1) { //check if the future position already has a cube
            tmpPiece.futurePosition = tmpPiece.position; //colision, future location disregarded
            return true;
        }  
        
        for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++) { //for every cube of the walls 
            if (sum.x == cubePositions[i].x && sum.y == cubePositions[i].y) {
                tmpPiece.futurePosition = tmpPiece.position; //colision, future location disregarded
                return true;
            }
        }

        
    }
    tmpPiece.position = tmpPiece.futurePosition; //no colision, piece moves for future location
    return false;
}



//rotation from/to. To be used with the offsetMatrix
//f.e rotation from spawn to clockwise: {0,1}
//f.e rotation from spawn to conterclockwise: {0,3}
int rotation[] = { 0,0 };  

//to rotate the pieces, and apply the offsets if necessary
void rotationFunc(bool dir) { //dir == true -> counter clockwise
    int x = 0; //if clockwise or counterclockwise
    rotation[0] = rotation[1];
    if (dir) {
        x = -1; //counter clockwise
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

    printf("from %d - to: %d\n", rotation[0], rotation[1]);

    //offsets
    int offset_n = 0; //offset number | after 4 tries doesn't rotate
    
    while (checkColision(true)) {
        if (offset_n > 4) { //none offset worked, ignore rotation
            rotationFunc(-dir); //rotate back to original form
            break;
        }
        printf("It indeed has colision!!!!\n\n");
        //I piece
        if (option == 0) tmpPiece.futurePosition = tmpPiece.futurePosition + offsetMatrixI[rotation[0], rotation[1]][offset_n];
        //remaining pieces
        else tmpPiece.futurePosition = tmpPiece.futurePosition + offsetMatrix[rotation[0], rotation[1]][offset_n]; //tries with offset
        offset_n++;
        printf("new coordenates: %f %f\n", tmpPiece.futurePosition.x, tmpPiece.futurePosition.y);
        //continue;
    }
    //there was no colision or the offset worked
    offset_n = 0;
    tmpPiece.position = tmpPiece.futurePosition;
        
    
}


void wait() {
    glfwWaitEventsTimeout(0.7);
}

void newPiece() {
    option =  (rand() % 7); //choose random piece (0-6)
    for(unsigned int i = 0; i<4; i++) tmpPiece.coord[i] = allPieces[option][i];
    tmpPiece.position = pieceSpawnPosition;
    tmpPiece.futurePosition = pieceSpawnPosition;
    tmpPiece.rot = 0;
    printf("new piece\n");
    //currentOption = option;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//register the piece on the board (registers the index of the color of the cube)
void registerPiece() {
    //printf("register\n");
    for (unsigned int i = 0; i < 4; i++) { //for every cube of the piece
        vec2 sum = tmpPiece.position + tmpPiece.coord[i]; //coordenate of the cube
        allPiecesInBoard[int(sum.y)-1][int(sum.x)-1] = option;
    }
    newPiece();
}


thread moveThread;
thread checkLineThread;
mutex mtx;

//unique_lock<mutex> lck(mtx, try_to_lock);
//bool gotLock = lck.owns_lock();
bool gotLock = true;

bool threads = true;
bool moving = false;
void movePieceDown() {
    while (threads) {
        this_thread::sleep_for(chrono::milliseconds(300));
        while(true){
            if (mtx.try_lock()) break;
        }
        gotLock = true;
        //gotLock = true;
        tmpPiece.futurePosition.y = tmpPiece.futurePosition.y - 1.0f;
        //printf("new position: %f %f\n", tmpPiece.futurePosition.x, tmpPiece.futurePosition.y);
        if ((tmpPiece.position - tmpPiece.futurePosition == vec3(0.0f, 1.0f, 0.0f)) && checkColision()) registerPiece();
        mtx.unlock();
        gotLock = false;
    }   

}


void moveOneRowDown(int row) {
    int current_row = row, next_row = row+1;
    for (unsigned int r = row+1; r < 20; r++) {
        for (unsigned int c = 0; c < 10; c++) {
            allPiecesInBoard[current_row][c] = allPiecesInBoard[next_row][c];
        }
        next_row++;
        current_row++;
    }
}

void checkLine() {
    while (threads) {
        int complete = 0;
        for (int r = 0; r < 21; r++) {
            for (unsigned int c = 0; c < 10; c++) {
                if (allPiecesInBoard[r][c] == -1) {
                    break;
                }
                else {
                    complete++;
                }
            }
            if (complete == 10) {
                printf("Line completed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                points++;
                moveOneRowDown(r);
                r = -1; //restart
            }
            complete = 0;
        }
    }
}

void printMatrix() {
    printf("-----------------MATRIX--------------------\n");
    for (int r = 20; r > -1; r--) {
        for (int c = 0; c < 10; c++) {
            printf("%d ", allPiecesInBoard[r][c]);
        }
        printf("\n");
    }
}


int main(){
    int record = 0;
    fstream recordFile("record.txt");
    recordFile >> record; // gets the record value on the file
    recordFile.close();

    srand(time(0)); //allow semi-random numbers
    
    //fill allPiecesInBoard matrix with 0's
    for (unsigned int r = 0; r < 21; r++) { 
        for (unsigned int c = 0; c < 10; c++) {
            allPiecesInBoard[r][c] = -1;
        }
    }


    
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
    //GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tetris 3D", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
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
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader basicShader("shaders/pieces.vs", "shaders/pieces.fs");
    Shader textShader("shaders/text.vs", "shaders/text.fs");
    

    //-----------------------------------------------------------WRITE ON SCREEN-------------------------------------------------------------------------
    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
        cout << "ERROR::FREETYPE: Failed to load font" << endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
                continue;
            }
            // generate texture
            unsigned int texture1;
            glGenTextures(1, &texture1);
            glBindTexture(GL_TEXTURE_2D, texture1);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture1,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    //-----------------------------------------------------------WRITE ON SCREEN-------------------------------------------------------------------------

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);
    // vertices position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture = loadTexture("textures/blockBW.png");
    basicShader.use();
    basicShader.setInt("texture1", 1);

    nextOption = -1;
    newPiece();

    moveThread = thread(movePieceDown);
    checkLineThread = thread(checkLine);
    //terminate(t);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        //reduce piece position 
        //tmpPiece.futurePosition.y = tmpPiece.futurePosition.y - 0.01;


        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = ortho(-(float)(SCR_WIDTH)/100, (float)(SCR_WIDTH)/100, -(float)(SCR_HEIGHT)/100, (float)(SCR_HEIGHT)/100);

        textShader.use();
        textShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(cubeVAO);

        projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        //-----------------------------------------frame-------------------------------------------------------
        basicShader.use();
        basicShader.setMat4("projection", projection);
        basicShader.setMat4("view", view);

        for (unsigned int i = 0; i < size(cubePositions); i++) { // draw square frame
            basicShader.setVec3("color", cubeColors[i]);
            model = translate(mat4(1.0f), cubePositions[i]);
            basicShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //-----------------------------------------pieces-------------------------------------------------------
        for (unsigned int i = 0; i < 4; i++){ // draw current piece
            basicShader.setVec3("color", allPiecesColors[option][0], allPiecesColors[option][1], allPiecesColors[option][2]);

            model = translate(mat4(1.0f), tmpPiece.position); //current position of the center piece
            model = translate(model, tmpPiece.coord[i]);//mount pieces together
          
            basicShader.setMat4("model", model);
            //printf("%d\n", i);
            //printf("%f %f %f %f\n\n", model[0][0], model[0][1], model[1][0], model[1][1]);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //-----------------------------------------previous pieces-------------------------------------------------------
        for (unsigned int r = 0; r < 21; r++) { //for every row   
            for (unsigned int c = 0; c < 10; c++) { //for every column
                if (allPiecesInBoard[r][c] == -1) continue; //position has no cube, jump to next column
                int index = allPiecesInBoard[r][c]; //index of the color
                //printf("%d %d %d\n", index, r, c);
                basicShader.setVec3("color", allPiecesColors[index][0], allPiecesColors[index][1], allPiecesColors[index][2]);
                vec3 pos = vec3(float(c+1), float(r+1), 0.0f);
                model = translate(mat4(1.0f), pos); //current position of the center piece
                //model = translate(model, tmpPiece.coord[i]);//mount pieces together

                basicShader.setMat4("model", model);
                //printf("%d\n", i);
                //printf("%f %f %f %f\n\n", model[0][0], model[0][1], model[1][0], model[1][1]);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        RenderText(textShader, "Record: " + to_string(record), -((float)(SCR_WIDTH) / 100) + 1, ((float)(SCR_HEIGHT) / 100) - 2, 0.03f, glm::vec3(0.0, 0.0f, 0.0f));

        RenderText(textShader, "Pontos: " + to_string(points), -((float)(SCR_WIDTH) / 100) + 1, -((float)(SCR_HEIGHT) / 100) + 1, 0.03f, glm::vec3(0.0, 0.0f, 0.0f));

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);

    // saves record
    if (points >= record) {
        fstream recordFile("record.txt");
        recordFile << points; 
        recordFile.close();
    }

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
        printf("type: %s\n", typeid(tmpPiece.coord[i].x).name());


    printf("-------------------------------------------------------\n");
    printMatrix();
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { 
        threads = false;
        moveThread.join();
        checkLineThread.join();
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(UPWARD, deltaTime + 0.05);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(DOWNWARD, deltaTime + 0.05);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (mtx.try_lock()) {
            tmpPiece.futurePosition.x -= speed;
            //check if future position is valid (if creates collision)
            checkColision();
            mtx.unlock();
            //this_thread::sleep_for(chrono::milliseconds(100));

            wait();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { 
        if (mtx.try_lock()) {
            tmpPiece.futurePosition.x += speed;
            //check if future position is valid (if creates collision)
            checkColision();
            mtx.unlock();
            //this_thread::sleep_for(chrono::milliseconds(100));
            
            wait();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (mtx.try_lock()) {
            tmpPiece.futurePosition.y -= speed;
            //check if future position is valid (if creates collision)
            checkColision();
            mtx.unlock();
            

            wait();
        }
    }
    
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && option != 2) { //clockwise (option 2 == cube, ergo no rotation)
        rotationFunc(false);
        wait(); 
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && option != 2) { //counter clockwise
        rotationFunc(true);
        wait(); 
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        debugFunction();
        wait();
    }

    //if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { registerPiece(); wait(); }
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
        cout << "Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }

    return textureID;
}
// render line of text
// -------------------
void RenderText(Shader& textShader, string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state  
    textShader.use();
    textShader.setVec3("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // iterate through all characters
    string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float textVertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(textVertices), textVertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
