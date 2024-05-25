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

void setupSSBO(Octree octreeArr[ARR_SIZE], unsigned int shaderProgram)
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
}

vec3 positions[8] = {
    {0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1},
    {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1},
};

void closest_point_on_aabb(vec3 point, vec3 min, vec3 max, vec3 closestRes) {
    vec3 closest;
    for (int i = 0; i < 3; ++i) {
        if (point[i] < min[i]) {
            closest[i] = min[i];
        } else if (point[i] > max[i]) {
            closest[i] = max[i];
        } else {
            closest[i] = point[i];
        }
    }
    vec3_add(closestRes, closestRes, closest);
}

int sphere_intersects_aabb(vec3 sphere_center, float sphere_radius, vec3 aabb_min, vec3 aabb_max) {
    vec3 closest_point = {0, 0, 0};
    closest_point_on_aabb(sphere_center, aabb_min, aabb_max, closest_point);
    vec3 difference;
    vec3_sub(difference, closest_point, sphere_center);
    float distance_squared = vec3_len(difference);
    return distance_squared <= sphere_radius * sphere_radius;
}

void destroyCells(OctreeArray *octarr, int rootInd, vec3 pos, vec3 sp, float sr, int depth)
{
    vec3 posM, size = {5.0 / powf(2.0, depth), 5.0 / powf(2.0, depth)};
    vec3_dup(posM, pos);
    vec3_add(posM, posM, size);
    if(!sphere_intersects_aabb(sp, sr, pos, posM)) return;
    Octree *root = &octarr->arr[rootInd];
    if (depth == DEPTH)
    {
        root->isColored = 0;
        root->isIntact = 1;
        return;
    }
    if (!root->isIntact)
    {
        for (int i = 0; i < 8; i++)
        {
            vec3 np = {0, 0, 0};
            vec3_scale(np, positions[i], size[0] / 2.0);
            vec3_add(np, np, pos);
            destroyCells(octarr, root->children[i], np, sp, sr, depth + 1);
        }
        return;
    }
    root->isIntact = 0;
    for (int i = 0; i < 8; i++)
    {
        vec3 np = {0, 0, 0};
        vec3_scale(np, positions[i], size[0] / 2.0);
        vec3_add(np, np, pos);
        Octree n;
        n.isColored = 1;
        n.isIntact = 1;
        root->children[i] = octarr_add(octarr, n);
        destroyCells(octarr, root->children[i], np, sp, sr, depth + 1);
    }
}