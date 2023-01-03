#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

//walls coordinates
vec3 cubePositions[] = { // POSITIONS OF CUBES IN FRAME
        vec3(0.0f,  0.0f,  0.0f), // base
        vec3(1.0f,  0.0f,  0.0f),
        vec3(2.0f,  0.0f,  0.0f),
        vec3(3.0f,  0.0f,  0.0f),
        vec3(4.0f,  0.0f,  0.0f),
        vec3(5.0f,  0.0f,  0.0f),
        vec3(6.0f,  0.0f,  0.0f),
        vec3(7.0f,  0.0f,  0.0f),
        vec3(8.0f,  0.0f,  0.0f),
        vec3(9.0f,  0.0f,  0.0f),
        vec3(10.0f,  0.0f,  0.0f),
        vec3(11.0f,  0.0f,  0.0f),

        vec3(11.0f,  1.0f,  0.0f), // coluna direita
        vec3(11.0f,  2.0f,  0.0f),
        vec3(11.0f,  3.0f,  0.0f),
        vec3(11.0f,  4.0f,  0.0f),
        vec3(11.0f,  5.0f,  0.0f),
        vec3(11.0f,  6.0f,  0.0f),
        vec3(11.0f,  7.0f,  0.0f),
        vec3(11.0f,  8.0f,  0.0f),
        vec3(11.0f,  9.0f,  0.0f),
        vec3(11.0f,  10.0f,  0.0f),
        vec3(11.0f,  11.0f,  0.0f),
        vec3(11.0f,  12.0f,  0.0f),
        vec3(11.0f,  13.0f,  0.0f),
        vec3(11.0f,  14.0f,  0.0f),
        vec3(11.0f,  15.0f,  0.0f),
        vec3(11.0f,  16.0f,  0.0f),
        vec3(11.0f,  17.0f,  0.0f),
        vec3(11.0f,  18.0f,  0.0f),
        vec3(11.0f,  19.0f,  0.0f),
        vec3(11.0f,  20.0f,  0.0f),
        vec3(11.0f,  21.0f,  0.0f),

        vec3(11.0f,  22.0f,  0.0f), // top
        vec3(10.0f,  22.0f,  0.0f),
        vec3(9.0f,  22.0f,  0.0f),
        vec3(8.0f,  22.0f,  0.0f),
        vec3(7.0f,  22.0f,  0.0f),
        vec3(6.0f,  22.0f,  0.0f),
        vec3(5.0f,  22.0f,  0.0f),
        vec3(4.0f,  22.0f,  0.0f),
        vec3(3.0f,  22.0f,  0.0f),
        vec3(2.0f,  22.0f,  0.0f),
        vec3(1.0f,  22.0f,  0.0f),
        vec3(0.0f,  22.0f,  0.0f),

        vec3(0.0f,  21.0f,  0.0f),
        vec3(0.0f,  20.0f,  0.0f),
        vec3(0.0f,  19.0f,  0.0f),
        vec3(0.0f,  18.0f,  0.0f), // coluna esquerda
        vec3(0.0f,  17.0f,  0.0f),
        vec3(0.0f,  16.0f,  0.0f),
        vec3(0.0f,  15.0f,  0.0f),
        vec3(0.0f,  14.0f,  0.0f),
        vec3(0.0f,  13.0f,  0.0f),
        vec3(0.0f,  12.0f,  0.0f),
        vec3(0.0f,  11.0f,  0.0f),
        vec3(0.0f,  10.0f,  0.0f),
        vec3(0.0f,  9.0f,  0.0f),
        vec3(0.0f,  8.0f,  0.0f),
        vec3(0.0f,  7.0f,  0.0f),
        vec3(0.0f,  6.0f,  0.0f),
        vec3(0.0f,  5.0f,  0.0f),
        vec3(0.0f,  4.0f,  0.0f),
        vec3(0.0f,  3.0f,  0.0f),
        vec3(0.0f,  2.0f,  0.0f),
        vec3(0.0f,  1.0f,  0.0f),
};


//-----------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------all the pieces relative coordinates------------------------------------------
vec3 pieceL[] = {
        vec3(0.0f,  0.0f,  0.0f),
        vec3(1.0f,  0.0f,  0.0f),
        vec3(-1.0f,  0.0f,  0.0f),
        vec3(-1.0f,  1.0f,  0.0f),
};
vec3 pieceI[] = {
    vec3(0.0f,  0.0f,  0.0f),
    vec3(-1.0f,  0.0f,  0.0f),
    vec3(-2.0f,  0.0f,  0.0f),
    vec3(1.0f,  0.0f,  0.0f),
};
vec3 pieceSquare[] = {
    vec3(0.0f,  0.0f,  0.0f),
    vec3(1.0f,  0.0f,  0.0f),
    vec3(0.0f,  1.0f,  0.0f),
    vec3(1.0f,  1.0f,  0.0f),
};
vec3 pieceLopposite[] = {
    vec3(0.0f,  0.0f,  0.0f),
    vec3(1.0f,  0.0f,  0.0f),
    vec3(-1.0f,  0.0f,  0.0f),
    vec3(1.0f,  1.0f,  0.0f),
};
vec3 pieceZ[] = {
    vec3(0.0f,  0.0f,  0.0f),
    vec3(1.0f,  0.0f,  0.0f),
    vec3(0.0f,  1.0f,  0.0f),
    vec3(-1.0f,  1.0f,  0.0f),
};
vec3 pieceZopposite[] = {
    vec3(0.0f,  0.0f,  0.0f),
    vec3(-1.0f,  0.0f,  0.0f),
    vec3(0.0f,  1.0f,  0.0f),
    vec3(1.0f,  1.0f,  0.0f),
};
vec3 pieceStair[] = {
    vec3(0.0f,  0.0f,  0.0f),
    vec3(1.0f,  0.0f,  0.0f),
    vec3(-1.0f,  0.0f,  0.0f),
    vec3(0.0f,  1.0f,  0.0f),
};


vec3* allPieces[] = {
    pieceI,pieceL,pieceSquare,pieceLopposite,pieceZ,pieceZopposite,pieceStair
};


//-----------------------------------------------------------------------------------------------------------------------------

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
    *offsetNull,*offset01,*offsetNull,*offset23,
    *offset10, *offsetNull, *offset10, *offsetNull,
    *offsetNull, *offset01, *offsetNull, *offset23,
    *offset32, *offsetNull, *offset32, *offsetNull
};





//-----------------------------------------------
// ------------------------------------------------------------------
float vertices[] = { // CUBE
    //posi�ao (x,y,z)        texture box
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
