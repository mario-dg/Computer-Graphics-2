/**
 * @file
 * Erstellt Axis Aligned Bounding Boxes und Object Oriented Bounding Boxes von Objekten
 *
 * @author Christopher Ploog, Mario da Graca
 */
#include "boundingBox.h"


boundingBox boundingBox_calculateAABB(object currObj, corners* corner) {
    boundingBox result;
    //Initialwert zum verlgeichen
    result.cube.cornerX.min = FLT_MAX;
    result.cube.cornerX.max = -FLT_MAX;
    result.cube.cornerY.min = FLT_MAX;
    result.cube.cornerY.max = -FLT_MAX;
    result.cube.cornerZ.min = FLT_MAX;
    result.cube.cornerZ.max = -FLT_MAX;

    //Ueber alle Vertizes iterieren
    for (int i = 0; i < currObj.vertexCount; ++i) {
        vec3 currVert;
        glm_vec3_copy(currObj.vertices[i], currVert);

        //x Min und Max
        result.cube.cornerX.min = fminf(currVert[0], result.cube.cornerX.min);
        result.cube.cornerX.max = fmaxf(currVert[0], result.cube.cornerX.max);

        //y Min und Max
        result.cube.cornerY.min = fminf(currVert[1], result.cube.cornerY.min);
        result.cube.cornerY.max = fmaxf(currVert[1], result.cube.cornerY.max);

        //z Min und Max
        result.cube.cornerZ.min = fminf(currVert[2], result.cube.cornerZ.min);
        result.cube.cornerZ.max = fmaxf(currVert[2], result.cube.cornerZ.max);
    }

    //Dimensionen
    result.width = result.cube.cornerX.max - result.cube.cornerX.min;
    result.height = result.cube.cornerY.max - result.cube.cornerY.min;
    result.depth = result.cube.cornerZ.max - result.cube.cornerZ.min;

    //Alle 8 Ecken eines Würfels aus den min und max Werten erstellen
    glm_vec3_copy((vec3){result.cube.cornerX.min, result.cube.cornerY.max, result.cube.cornerZ.max}, corner->topFrontLeft);
    glm_vec3_copy((vec3){result.cube.cornerX.max, result.cube.cornerY.max, result.cube.cornerZ.max}, corner->topFrontRight);
    glm_vec3_copy((vec3){result.cube.cornerX.min, result.cube.cornerY.min, result.cube.cornerZ.max}, corner->bottomFrontLeft);
    glm_vec3_copy((vec3){result.cube.cornerX.max, result.cube.cornerY.min, result.cube.cornerZ.max}, corner->bottomFrontRight);
    glm_vec3_copy((vec3){result.cube.cornerX.min, result.cube.cornerY.max, result.cube.cornerZ.min}, corner->topBackLeft);
    glm_vec3_copy((vec3){result.cube.cornerX.max, result.cube.cornerY.max, result.cube.cornerZ.min}, corner->topBackRight);
    glm_vec3_copy((vec3){result.cube.cornerX.min, result.cube.cornerY.min, result.cube.cornerZ.min}, corner->bottomBackLeft);
    glm_vec3_copy((vec3){result.cube.cornerX.max, result.cube.cornerY.min, result.cube.cornerZ.min}, corner->bottomBackRight);
    
    return result;
}

object boundingBox_createObjectFromBoundingBox(corners corner) {
    object result;

    //Rechteck aus 12 Dreiecken erstellen
    result.vertexCount = 8;
    result.vertices = (vec3 *) calloc(result.vertexCount, sizeof(vec3));
    glm_vec3_copy(corner.bottomFrontLeft, result.vertices[0]);
    glm_vec3_copy(corner.bottomFrontRight, result.vertices[1]);
    glm_vec3_copy(corner.topFrontLeft, result.vertices[2]);
    glm_vec3_copy(corner.topFrontRight, result.vertices[3]);
    glm_vec3_copy(corner.topBackLeft, result.vertices[4]);
    glm_vec3_copy(corner.topBackRight, result.vertices[5]);
    glm_vec3_copy(corner.bottomBackLeft, result.vertices[6]);
    glm_vec3_copy(corner.bottomBackRight, result.vertices[7]);

    //Indizes
    result.faceCount = 12;
    result.facesTM = (triangleTM *) calloc(result.faceCount, sizeof(triangleTM));
    faces indices[] = {{0, 1, 2},
                       {2, 1, 3},
                       {2, 3, 4},
                       {4, 3, 5},
                       {4, 5, 6},
                       {6, 5, 7},
                       {6, 7, 0},
                       {0, 7, 1},
                       {1, 7, 3},
                       {3, 7, 5},
                       {6, 0, 4},
                       {4, 0, 2}};

    for (int i = 0; i < result.faceCount; ++i) {
        glm_vec3_copy(result.vertices[indices[i].index1], result.facesTM[i].vertices.a);
        glm_vec3_copy(result.vertices[indices[i].index2], result.facesTM[i].vertices.b);
        glm_vec3_copy(result.vertices[indices[i].index3], result.facesTM[i].vertices.c);
        utils_calcTwoEdgesTM(&result.facesTM[i]);
        utils_calcNormal(&result.facesTM[i]);
    }

    return result;
}

void boundingBox_createOOBFromAABB(boundingBox aabb, object obj, corners *corner, vec3 bunnyTranslation) {
    vec3 center= {0.0f, 0.0f, 0.0f};

    //OOBB um den Mittelpunkt erstellen
    vec3 bunnyAtCenter[obj.vertexCount];

    //Komponentenweise Mittelung der Vertizes, um den Mittelpunkt des Hasen zu bestimmen
    for (int i = 0; i < obj.vertexCount; ++i) {
        glm_vec3_copy(obj.vertices[i], bunnyAtCenter[i]);
        glm_vec3_sub(bunnyAtCenter[i], bunnyTranslation, bunnyAtCenter[i]);

        center[0] += bunnyAtCenter[i][0];
        center[1] += bunnyAtCenter[i][1];
        center[2] += bunnyAtCenter[i][2];
    }

    center[0] /= (float) obj.vertexCount;
    center[1] /= (float) obj.vertexCount;
    center[2] /= (float) obj.vertexCount;


    GLfloat minXDist = FLT_MAX, maxXDist = -FLT_MAX;
    GLfloat minZDist = FLT_MAX, maxZDist = -FLT_MAX;
    GLfloat minArea = FLT_MAX;
    GLfloat angle = 0.0f;

    for (int i = 0; i <= 175; i += 5) {
        //Geraden in x und z Richtung
        vec3 vecX = {1.0f, 0.0f, 0.0f};
        vec3 vecZ = {0.0f, 0.0f, 1.0f};

        //Geraden rotieren
        glm_vec3_rotate(vecX, glm_rad((float) i), (vec3){0.0f, 1.0f, 0.0f});
        glm_vec3_rotate(vecZ, glm_rad((float) i), (vec3){0.0f, 1.0f, 0.0f});

        //An Center Position des Bunnys translatieren und normieren
        glm_vec3_add(vecX, center, vecX);
        glm_vec3_normalize(vecX);

        glm_vec3_add(vecZ, center, vecZ);
        glm_vec3_normalize(vecZ);

        GLfloat minXDistTemp = FLT_MAX, maxXDistTemp = -FLT_MAX;
        GLfloat minZDistTemp = FLT_MAX, maxZDistTemp = -FLT_MAX;

        for (int k = 0; k < obj.vertexCount; ++k) {
            vec3 PM;
            glm_vec3_sub(bunnyAtCenter[k], center, PM);

            //Min Max Distanz auf der x-Achse
            GLfloat distX = glm_vec3_dot(vecX, PM);
            minXDistTemp = fminf(distX, minXDistTemp);
            maxXDistTemp = fmaxf(distX, maxXDistTemp);

            //Min Max Distanz auf der z-Achse
            GLfloat distZ = glm_vec3_dot(vecZ, PM);
            minZDistTemp = fminf(distZ, minZDistTemp);
            maxZDistTemp = fmaxf(distZ, maxZDistTemp);
        }

        //Flaecheninhalt bestimmen
        GLfloat minAreaTemp = (fabsf(maxZDistTemp) + fabsf(minZDistTemp)) * (fabsf(maxXDistTemp) + fabsf(minXDistTemp));

        //Kleiner Flaecheninhalt bestimmt die optimale OOBB
        if(minAreaTemp < minArea){
            minXDist = minXDistTemp;
            minZDist = minZDistTemp;
            maxXDist = maxXDistTemp;
            maxZDist = maxZDistTemp;
            minArea = minAreaTemp;
            angle = (float) i;
        }

    }

    vec3 min = {minXDist, aabb.cube.cornerY.min, minZDist};
    vec3 max = {maxXDist, aabb.cube.cornerY.max, maxZDist};

    //Dimensionen der Bounding Box
    glm_vec3_copy((vec3){min[0], min[1], max[2]}, corner->bottomFrontLeft);
    glm_vec3_copy((vec3){max[0], min[1], max[2]}, corner->bottomFrontRight);
    glm_vec3_copy((vec3){min[0], min[1], min[2]}, corner->bottomBackLeft);
    glm_vec3_copy((vec3){max[0], min[1], min[2]}, corner->bottomBackRight);

    glm_vec3_copy((vec3){min[0], max[1], max[2]}, corner->topFrontLeft);
    glm_vec3_copy((vec3){max[0], max[1], max[2]}, corner->topFrontRight);
    glm_vec3_copy((vec3){min[0], max[1], min[2]}, corner->topBackLeft);
    glm_vec3_copy((vec3){max[0], max[1], min[2]}, corner->topBackRight);

    //Um den Winkel rotieren, der den kleinsten Flaecheninhalt liefert
    glm_vec3_rotate(corner->bottomFrontLeft, glm_rad(angle), (vec3){0.0f, 1.0f, 0.0f});
    glm_vec3_rotate(corner->bottomFrontRight, glm_rad(angle), (vec3){0.0f, 1.0f, 0.0f});
    glm_vec3_rotate(corner->bottomBackLeft, glm_rad(angle), (vec3){0.0f, 1.0f, 0.0f});
    glm_vec3_rotate(corner->bottomBackRight, glm_rad(angle), (vec3){0.0f, 1.0f, 0.0f});

    glm_vec3_rotate(corner->topFrontLeft, glm_rad(angle), (vec3){0.0f, 1.0f, 0.0f});
    glm_vec3_rotate(corner->topFrontRight, glm_rad(angle), (vec3){0.0f, 1.0f, 0.0f});
    glm_vec3_rotate(corner->topBackLeft, glm_rad(angle), (vec3){0.0f, 1.0f, 0.0f});
    glm_vec3_rotate(corner->topBackRight, glm_rad(angle), (vec3){0.0f, 1.0f, 0.0f});

    //An Position des Hasen translatieren und Bounding Box in der Szene positioniern
    glm_vec3_add(corner->bottomFrontLeft, (vec3){center[0] + bunnyTranslation[0], 0.0f, center[2] + bunnyTranslation[2]}, corner->bottomFrontLeft);
    glm_vec3_add(corner->bottomFrontRight,(vec3){center[0] + bunnyTranslation[0], 0.0f, center[2] + bunnyTranslation[2]}, corner->bottomFrontRight);
    glm_vec3_add(corner->bottomBackLeft,  (vec3){center[0] + bunnyTranslation[0], 0.0f, center[2] + bunnyTranslation[2]}, corner->bottomBackLeft);
    glm_vec3_add(corner->bottomBackRight, (vec3){center[0] + bunnyTranslation[0], 0.0f, center[2] + bunnyTranslation[2]}, corner->bottomBackRight);

    glm_vec3_add(   corner->topFrontLeft, (vec3){center[0] + bunnyTranslation[0], 0.0f, center[2] + bunnyTranslation[2]}, corner->topFrontLeft);
    glm_vec3_add(   corner->topFrontRight,(vec3){center[0] + bunnyTranslation[0], 0.0f, center[2] + bunnyTranslation[2]}, corner->topFrontRight);
    glm_vec3_add(   corner->topBackLeft,  (vec3){center[0] + bunnyTranslation[0], 0.0f, center[2] + bunnyTranslation[2]}, corner->topBackLeft);
    glm_vec3_add(   corner->topBackRight, (vec3){center[0] + bunnyTranslation[0], 0.0f, center[2] + bunnyTranslation[2]}, corner->topBackRight);
}