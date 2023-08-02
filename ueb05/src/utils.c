/**
 * @file
 * Stellt allgemeine Funktionen zur Verfuegung
 *
 * @author Christopher Ploog, Mario da Graca
 */

#include <string.h>
#include "utils.h"

Hit utils_createDefaultHit(void) {
    Hit result;
    result.defaultHit = GL_TRUE;
    return result;
}

void utils_transformVertices(vec3 vertex, vec3 translation, vec3 rotation, GLfloat scale){
    //Skalierung
    glm_vec3_scale(vertex, scale, vertex);

    //Rotation
    glm_vec3_rotate(vertex, glm_rad(rotation[0]), (vec3) {1,0,0}); //X
    glm_vec3_rotate(vertex, glm_rad(rotation[1]), (vec3) {0,1,0}); //Y
    glm_vec3_rotate(vertex, glm_rad(rotation[2]), (vec3) {0,0,1}); //Z

    //Translation
    glm_vec3_add(vertex, translation, vertex);
}

void utils_calcNormal(triangleTM *tri){
    glm_vec3_cross(tri->edge1, tri->edge2, tri->normal);
    glm_vec3_normalize(tri->normal);
}

void utils_calcTwoEdgesTM(triangleTM *tri){
    glm_vec3_sub(tri->vertices.b, tri->vertices.a, tri->edge1);
    glm_vec3_sub(tri->vertices.c, tri->vertices.a, tri->edge2);
}

void utils_reflectDir(vec3 ray, vec3 normal, vec3 result) {
    GLfloat val = 2 * glm_vec3_dot(ray, normal);
    vec3 temp;
    temp[0] = val * normal[0];
    temp[1] = val * normal[1];
    temp[2] = val * normal[2];

    glm_vec3_sub(ray, temp, result);
    glm_vec3_normalize(result);
}

void utils_attenuationFunction(Ray ray, Color *color) {
    //-log(x + 0.3) * 0.15 + 0.95
    GLfloat attenuation = -logf(ray.distance + 0.3f) * 0.15f + 0.95f;
    color->r *= attenuation;
    color->g *= attenuation;
    color->b *= attenuation;
}

GLfloat utils_colorIntensity(Color color) {
    vec3 color3f = {color.r, color.g, color.b};
    return (1.0f / 2.0f) * (glm_vec3_max(color3f) + glm_vec3_min(color3f));
}

char *utils_concatStrings(const char *s1, const char *s2){
    // +1 Nullterminator
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    if(result == NULL){
        printf("Couldn't concat strings\n");
        exit(1);
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}