#include "octree.h"
#include <stdlib.h>

int octarr_add(OctreeArray *octarr, Octree a){
    octarr->arr[octarr->pr] = a;
    return octarr->pr++;
}

OctreeArray *octarr_new(){
    OctreeArray *n = malloc(sizeof(OctreeArray));
    n->pr = 0;
    return n;
}

GLuint setupSSBO(Octree octreeArr[ARR_SIZE], unsigned int shaderProgram)
{
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Octree) * ARR_SIZE, octreeArr, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, "OctreeBuffer");
    glUniformBlockBinding(shaderProgram, blockIndex, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ssbo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return ssbo;
}

vec3 positions[8] = {
    {0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1},
    {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1},
};

void collapseVoxel(OctreeArray *octarr, int rootInd){
    for(int i = 0; i < 8; i++){
        Octree child = octarr->arr[octarr->arr[rootInd].children[i]];
        if(!child.isIntact || (
            child.isIntact && child.isColored
        )) return;
    }
    octarr->arr[rootInd].isIntact = 1;
    octarr->arr[rootInd].isColored = 0;
    octarr->arr[rootInd].isCollapsed = 1;
}

float squared(float v) { return v * v; }
int doesCubeIntersectSphere(vec3 C1, vec3 C2, vec3 S, float R)
{
    float dist_squared = R * R;
    /* assume C1 and C2 are element-wise sorted, if not, do that now */
    if (S[0] < C1[0]) dist_squared -= squared(S[0] - C1[0]);
    else if (S[0] > C2[0]) dist_squared -= squared(S[0] - C2[0]);
    if (S[1] < C1[1]) dist_squared -= squared(S[1] - C1[1]);
    else if (S[1] > C2[1]) dist_squared -= squared(S[1] - C2[1]);
    if (S[2] < C1[2]) dist_squared -= squared(S[2] - C1[2]);
    else if (S[2] > C2[2]) dist_squared -= squared(S[2] - C2[2]);
    return dist_squared > 0;
}

void destroyVoxels(OctreeArray *octarr, int rootInd, vec3 pos, vec3 sp, float sr, int depth)
{
    vec3 posM, size = {5.0 / powf(2.0, depth), 5.0 / powf(2.0, depth)};
    vec3_dup(posM, pos);
    vec3_add(posM, pos, size);
    if(!doesCubeIntersectSphere(pos, posM, sp, sr)) return;
    Octree *root = octarr->arr + rootInd;
    if (depth == DEPTH){
        root->isColored = 0;
        root->isIntact = 1;
        return;
    }
    if (!root->isIntact || root->isCollapsed){
        for (int i = 0; i < 8; i++){
            vec3 np = {0, 0, 0};
            vec3_scale(np, positions[i], size[0] / 2.0);
            vec3_add(np, np, pos);
            destroyVoxels(octarr, root->children[i], np, sp, sr, depth + 1);
        }
        collapseVoxel(octarr, rootInd);
        return;
    }
    root->isIntact = 0;
    for (int i = 0; i < 8; i++){
        vec3 np = {0, 0, 0};
        vec3_scale(np, positions[i], size[0] / 2.0);
        vec3_add(np, np, pos);
        Octree n;
        n.isColored = 1;
        n.isIntact = 1;
        n.isCollapsed = 0;
        root->children[i] = octarr_add(octarr, n);
        destroyVoxels(octarr, root->children[i], np, sp, sr, depth + 1);
    }
    collapseVoxel(octarr, rootInd);
}