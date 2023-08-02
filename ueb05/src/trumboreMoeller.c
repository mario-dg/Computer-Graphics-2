/**
 * @file
 * Implementiert den Trumbore Moeller Algorithmus um zu Testen, ob ein Strahl
 * mit einem Dreieck intersected
 * @author Christopher Ploog, Mario da Graca
 */

#include "trumboreMoeller.h"
#include "utils.h"

Hit trumboreMoeller_rayTriangleIntersection(Ray ray, triangleTM currTri) {
    Hit result = utils_createDefaultHit();

    vec3 tVec, pVec, qVec;
    //Determinante, inverse Determinante, Baryzentrische Koordinaten
    GLfloat det, invDet, u, v;

    //Determinante
    glm_vec3_cross(ray.dir, currTri.edge2, pVec);
    det = glm_vec3_dot(currTri.edge1, pVec);

    //Determinante == 0 -> Strahl parallel zum Dreieck
    if (fabsf(det) < EPSILON) return result;

    //Inverse Determinante
    invDet = 1.0f / det;

    //Vektor von Kamera-Position zu einer Ecke des Dreiecks
    glm_vec3_sub(ray.start, currTri.vertices.a, tVec);

    /** ------------------- BARYZENTRISCHE KOORDINATEN ------------------- */
    //u-Koordinate
    u = invDet * glm_vec3_dot(tVec, pVec);
    if ((u < 0.0f) || (u > 1.0f)) return result;

    //v-Koordinate
    glm_vec3_cross(tVec, currTri.edge1, qVec);
    v = invDet * glm_vec3_dot(ray.dir, qVec);
    if ((v < 0.0f) || (u + v > 1.0f)) return result;

    //Abstand von der Kamera-Position zum geschnittenen Punkt berechnen
    result.dist = invDet * glm_vec3_dot(currTri.edge2, qVec);

    if (result.dist > EPSILON) {
        result.defaultHit = GL_FALSE;
        result.position[0] = ray.start[0] + result.dist * ray.dir[0];
        result.position[1] = ray.start[1] + result.dist * ray.dir[1];
        result.position[2] = ray.start[2] + result.dist * ray.dir[2];
    }

    return result;
}