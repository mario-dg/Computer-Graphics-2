/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Logik-Funktionalitaet des Programms.
 *
 * @authors Mario Da Graca, Christopher Ploog
 */

#include <GL/gl.h>
#include <stdlib.h>

#include "logic.h"
#include "types.h"
#include "math.h"
#include "float.h"
#include "spline.h"
#include "bezier.h"
#include "convexHull.h"

//setzen von Globalen Variablen
GLint g_Spline;
GLint g_MaxSteps = DEFAULT_MAX_STEPS;
GLint g_AmntVerts = AMOUNT_VERT_1;
GLint g_AmntCurveVert;
GLint g_AmntConvexHullVert;

CGPoint2f *g_ControlVertices;
CGPoint2f *g_CurveVertices;
CGPoint2f *g_ConvexHullVertices;
CGPoint2f *g_Normals;

/**
 * Liefert den Index des am naechst gelegenden Punktes
 * @param mousePos Maus Position
 * @return Index des nahesten Punktes
 */
GLint getClosestIndex(CGPoint2f mousePos) {
    GLint closestIndex = 0;
    GLfloat closestDist = FLT_MAX;
    CGPoint2f currVert;

    //Laufen ueber alle Kontroll Punkte
    for (int i = 0; i < g_AmntVerts; ++i) {
        currVert[CCX] = g_ControlVertices[i][CCX];
        currVert[CCY] = g_ControlVertices[i][CCY];

        //Distanz zum Mauszeiger bestimmen
        GLfloat temp = hypotf(currVert[CCX] - mousePos[CCX], currVert[CCY] - mousePos[CCY]);

        //Falls die Distanz am kleinsten ist, Punkt uebernehmen
        if (temp < closestDist && temp <= CIRCLE_RADIUS_CONTROL) {
            closestDist = temp;
            closestIndex = i;
        }
    }

    return closestIndex;
}

/**
 * Berechnung der Normalen
 */
void calculateNormals() {
    CGPoint3f zVec = {0.0f, 0.0f, 1.0f};
    CGPoint3f normal;
    GLfloat len;

    GLint leftIdx = 0;
    GLint rightIdx = 0;

    //Berechnung der Normalen, die zwischen die zwischen dem ersten und letzen Kontrollpunkt liegen
    for (int i = 0; i < g_AmntCurveVert; ++i) {
        if(i == 0){
            leftIdx = 0;
            rightIdx = 1;
        } else if(i == g_AmntCurveVert - 1) {
            leftIdx = g_AmntCurveVert - 2;
            rightIdx = g_AmntCurveVert - 1;
        } else{
            leftIdx = i;
            rightIdx = i + 1;
        }
        CGPoint3f leftNeighbour = {g_CurveVertices[leftIdx][CCX], g_CurveVertices[leftIdx][CCY], 0.0f};
        CGPoint3f rightNeighbour = {g_CurveVertices[rightIdx][CCX], g_CurveVertices[rightIdx][CCY], 0.0f};
        CGPoint3f LRVec = {leftNeighbour[CCX] - rightNeighbour[CCX], leftNeighbour[CCY] - rightNeighbour[CCY], 0.0f};

        normal[0] = LRVec[1] * zVec[2] - LRVec[2] * zVec[1];
        normal[1] = LRVec[2] * zVec[0] - LRVec[0] * zVec[2];
        normal[2] = LRVec[0] * zVec[1] - LRVec[1] * zVec[0];

        //Normalisieren der Laenge der Normalen
        len = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
        normal[0] /= len;
        normal[1] /= len;

        g_Normals[i][CCX] = normal[0];
        g_Normals[i][CCY] = normal[1];
    }
}

/**
 * Position des angeklickten Vertex bewegen
 * @param x X-Pos des Mauszeigers
 * @param y Y-Pos des Mauszeigers
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @param closestIndex naehsgelegter Index
 */
void moveVertex(GLint x, GLint y, GLfloat width, GLfloat height, GLint closestIndex) {
    //Index valide
    if (closestIndex > 0) {

        GLfloat aspect = width / height;
        CGPoint2f mousePos;

        if (aspect <= 1) {
            mousePos[CCX] = (x - (width / 2.0f)) / (width / 2.0f);
            mousePos[CCY] = -1.0f * (y - (height / 2.0f)) / ((height / 2) * aspect);
        } else {
            mousePos[CCX] = (x - (width / 2.0f)) / (width / 2.0f) * aspect;
            mousePos[CCY] = -1.0f * (y - (height / 2.0f)) / (height / 2);
        }

        //Kontroll Punkt auf Maus Position setzen
        g_ControlVertices[closestIndex][CCX] = mousePos[CCX];
        g_ControlVertices[closestIndex][CCY] = mousePos[CCY];

        if (g_Spline) {
            //B-Spline zeichnen
            g_CurveVertices = spline(g_MaxSteps, g_ControlVertices, g_AmntVerts);
        } else {
            //Bezier Kurve Zeichenen
            g_CurveVertices = bezier(g_MaxSteps, g_ControlVertices, g_AmntVerts);
        }

        calcConvexHull();
        calculateNormals();
    }
}

/**
 * Berechnung der Konvexen Huelle
 */
void calcConvexHull() {
    CGPoint controlVerts[g_AmntVerts];

    //Uebertragen der Vertizes in anderen Struct
    for (int i = 0; i < g_AmntVerts; ++i) {
        CGPoint pt = {g_ControlVertices[i][CCX], g_ControlVertices[i][CCY]};
        controlVerts[i] = pt;
    }
    free(g_ConvexHullVertices);
    //Neue Position der Konvex-Hull-Verts berechnen
    g_ConvexHullVertices = convexHull(controlVerts, g_AmntVerts, &g_AmntConvexHullVert);
}

/**
 * Initialisieren der Logik
 */
void init() {
    g_Spline = 1;
    g_AmntCurveVert = (g_AmntVerts - 3) * g_MaxSteps + 1;
    //Platz fuer Kontroll-Verts alloziieren
    g_ControlVertices = malloc(sizeof(CGPoint2f) * g_AmntVerts);
    if (!g_ControlVertices) {
        exit(1);
    }

    //Platz fuer Normals alloziieren
    g_Normals = malloc(sizeof(CGPoint2f) * g_AmntCurveVert);
    if (!g_Normals) {
        free(g_ControlVertices);
        exit(1);
    }
    //Kontrol-Verts Initialisieren
    for (int i = 0; i < g_AmntVerts; ++i) {
        g_ControlVertices[i][CCX] =
                (-1 + DEFAULT_SPACING) + ((float) i * ((2.0f - 2 * DEFAULT_SPACING) / (g_AmntVerts - 1.0f)));
        g_ControlVertices[i][CCY] = (float) (i % 2) * 0.2f;
    }

    g_CurveVertices = spline(g_MaxSteps, g_ControlVertices, g_AmntVerts);
    if (!g_CurveVertices) {
        free(g_ControlVertices);
        exit(1);
    }

    calculateNormals();
    calcConvexHull();
}

/**
 * Anpassung der Schritte zwischen Kontroll Verts
 * @param v zu veraendernder Wert (+1 / -1)
 * @return Error code (ok=1; Error=0)
 */
GLint adjustSteps(GLint v) {
    GLint error = 1;
    if (g_MaxSteps + v < MAX_STEPS && g_MaxSteps + v > MIN_STEPS) {
        g_MaxSteps += v;
        g_AmntCurveVert = (g_AmntVerts - 3) * g_MaxSteps + 1;
        g_AmntCurveVert = (g_AmntVerts - 3) * g_MaxSteps + 1;

        if (g_Spline) {
            g_CurveVertices = realloc(g_CurveVertices, sizeof(CGPoint2f) * g_AmntCurveVert);
            g_CurveVertices = spline(g_MaxSteps, g_ControlVertices, g_AmntVerts);
        } else {
            g_CurveVertices = realloc(g_CurveVertices, sizeof(CGPoint2f) * g_AmntCurveVert);
            g_CurveVertices = bezier(g_MaxSteps, g_ControlVertices, g_AmntVerts);
        }
    } else {
        error = 0;
    }
    return error;
}


GLint getAmountVertices() {
    return g_AmntVerts;
}

GLint getAmountCurveVertices() {
    return g_AmntCurveVert;
}

GLint getAmountConvexHullVertices() {
    return g_AmntConvexHullVert;
}

CGPoint2f *getControlVertices() {
    return g_ControlVertices;
}

CGPoint2f *getCurveVertices() {
    return g_CurveVertices;
}

CGPoint2f *getConvexHullVertices() {
    return g_ConvexHullVertices;
}

/**
 * Wechsel ziwchen Bezier und Spline
 */
void toggleSplineBezier() {
    g_Spline = !g_Spline;
    if (!g_Spline) {
        g_CurveVertices = realloc(g_CurveVertices, sizeof(CGPoint2f) * g_AmntCurveVert);
        g_CurveVertices = bezier(g_MaxSteps, g_ControlVertices, g_AmntVerts);
    } else {
        g_CurveVertices = realloc(g_CurveVertices, sizeof(CGPoint2f) * g_AmntCurveVert);
        g_CurveVertices = spline(g_MaxSteps, g_ControlVertices, g_AmntVerts);
    }
}

GLint drawSplineBezier() {
    return g_Spline;
}

CGPoint2f *getNormals() {
    return g_Normals;
}