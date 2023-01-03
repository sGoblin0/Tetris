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