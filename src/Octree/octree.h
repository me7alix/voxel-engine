#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../Glad/glad.h"
#include "../linmath.h"

#define ARR_SIZE 10000
#define DEPTH 4

typedef struct {
    int children[8];
    int isIntact;
    int isColored;
} Octree;

typedef struct {
    Octree arr[ARR_SIZE];
    int pr;
} OctreeArray;

int octarr_add(OctreeArray *octarr, Octree a);
void setupSSBO(Octree octreeArr[ARR_SIZE], unsigned int shaderProgram);
void boxInter(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax, vec2 *result);
void destroyCells(OctreeArray *octarr, int rootInd, vec3 pos, vec3 sp, float sr, int depth);
OctreeArray *octarr_new();