/**
 * @file
 * Schnittstelle, um .obj Dateien zu laden und Szenenobjekte daraus zu erstellen
 *
 * @author Christopher Ploog, Mario da Graca
 */

#include <string.h>
#include <stdlib.h>
#include "loadObj.h"
#include "stdio.h"
#include "sceneObjects.h"

/** Dateipfad zu den obj Dateien */
static const char* FILE_PATH = "../res/model/";

void loadObj_createObjectFromFile(vec3 *vertices, object *currObj, faces *indices, vec3 translation, vec3 rotation,
                                  GLfloat scale) {
    currObj->facesTM = calloc(currObj->faceCount, sizeof(struct triangleTM));
    currObj->vertices = calloc(currObj->vertexCount, sizeof(vec3));

    for (int j = 0; j < currObj->vertexCount; ++j) {
        //Alle Vertizes transformieren
        utils_transformVertices(vertices[j], translation, rotation, scale);
        glm_vec3_copy(vertices[j], currObj->vertices[j]);
    }

    for (int i = 0; i < currObj->faceCount; ++i) {
        //Dreieck aus 3 Vertizes erzeugen
        glm_vec3_copy(currObj->vertices[indices[i].index1], currObj->facesTM[i].vertices.a);
        glm_vec3_copy(currObj->vertices[indices[i].index2], currObj->facesTM[i].vertices.b);
        glm_vec3_copy(currObj->vertices[indices[i].index3], currObj->facesTM[i].vertices.c);
        //Zwei Seiten des Dreiecks vorberechnen
        //-> spart Rechenleistung beim Schnittvergleich Strahl und Dreieck
        utils_calcTwoEdgesTM(&currObj->facesTM[i]);
        //Normale des Dreiecks vorberechnen
        //-> spart Rechenleistung beim rendern der Szene selber
        utils_calcNormal(&currObj->facesTM[i]);
    }
}

object loadObj_readFile(const char *fileName, vec3 translation, vec3 rotation, GLfloat scale) {
    
    object result = sceneObjects_initDefaultModel();
    
    //Aus dem Dateinamen und dem Pfad die Datei laden
    char *path = utils_concatStrings(FILE_PATH, fileName);
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        printf("Couldn't open file!\n");
        return result;
    }

    GLint vertexCount = 0;
    GLint faceCount = 0;
    faces *indices;
    vec3 *vertices;

    //Kommentare auswerten
    //Anzahl der Vertizes
    if (fscanf(file, "# vertex count = %d\n", &result.vertexCount) != 1) {
        printf("Vertex Count Syntax-Error in .obj-File comment!\n");
    }
    vertices = calloc(result.vertexCount, sizeof(vec3));

    //Anzahl der Dreiecke
    if (fscanf(file, "# face count = %d\n", &result.faceCount) != 1) {
        printf("Face Count Syntax-Error in .obj-File comment!\n");
    }
    indices = calloc(result.faceCount, sizeof(faces));

    //Solange die Datei verarbeiten, bis EOF erreicht ist
    while (1) {

        //Ersten char aus der Zeile auslesen -> Vertex oder Face
        char lineHeader[64];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) {
            break;
        }

        if (strcmp(lineHeader, "v") == 0) {
            //Koordinaten des Vertexes auslesen
            if(fscanf(file, "%f %f %f\n", &vertices[vertexCount][0],
                   &vertices[vertexCount][1],
                   &vertices[vertexCount][2]) != 3){
                printf("ERROR in File for a Vertex! \t%d\n", vertexCount);
                exit(1);
            }
            vertexCount++;
        } else if (strcmp(lineHeader, "f") == 0) {
            //Indizes einlesen (.obj starten bei Index 1!)
            if(fscanf(file, "%d %d %d\n", &indices[faceCount].index1,
                   &indices[faceCount].index2,
                   &indices[faceCount].index3) != 3){
                printf("ERROR in File for a Face!\t%d\n", faceCount);
                exit(1);
            }

            indices[faceCount].index1--;
            indices[faceCount].index2--;
            indices[faceCount].index3--;
            faceCount++;
        }
    }

    //Objekt aus den ausgelesenen Datene erstellen
    loadObj_createObjectFromFile(vertices, &result, indices, translation, rotation, scale);
    
    if(result.vertexCount <= 0){
        printf("Error loading obj File!\n");
        exit(1);
    }
    
    free(path);
    fclose(file);
    return result;
}