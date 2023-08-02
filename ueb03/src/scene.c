/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Textausgabe mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else

#include <GL/gl.h>
#include <GL/glut.h>

#endif

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "debugGL.h"
#include "stringOutput.h"
#include "logic.h"
#include "texture.h"
#include "sceneObjects.h"
#include <math.h>

/**Kameraposition im KugelKoordinatensystem */
static GLfloat cameraRadiussph = 0.0f;
static GLfloat cameraThetaSph = 0.0f;
static GLfloat cameraPhiSph = 0.0f;


static GLuint *g_splineIndices;


static GLint usedTexture = 0;

/** Startposition der Kamera */
#define CAMERA_DEFAULT_RADIUS 3.0f
#define CAMERA_DEFAULT_THETA M_PI_4
#define CAMERA_DEFAULT_PHI M_PI_2


#ifndef DEG2RAD
/** Winkelumrechnung von Grad nach Radiant */
#define DEG2RAD(x) ((x) / 180.0f * M_PI)
#endif

/* Umschalten einiger Funktionen */
GLboolean showHelp = GL_FALSE;
GLboolean showWin = GL_FALSE;
GLboolean showLoss = GL_FALSE;
GLboolean showSpheres = GL_FALSE;
GLboolean showInterpolSpheres = GL_FALSE;

static controlVertex *g_controlVertices;
static splineVertex *g_splineSurface;

static CGPoint3f goalSphere;

/**
* Gibt den Hilfetext aus.
*/
static void drawHelp() {
    int size = 25;

    GLfloat color[3] = {1.0f, 1.0f, 0.8f};

    char *help[] = {"Hilfe",
                    "F1 - Wireframe an/aus",
                    "F2 - Normalen an/aus",
                    "F3 - Beleuchtungsberechnung an/aus",
                    "F4 - Punktlichtquelle an/aus",
                    "F6 - Kontroll-Sphaeren an/aus",
                    "F7 - Interpolierte-Sphaeren an/aus",
                    "F8 - Kugeln losrollen lassen/stoppen",
                    "F9 - Spiel neu starten",
                    "F10 - Erste Kugel anstoßen",
                    "F11 - De-/Aktiviert die Siegmoeglichkeit",
                    "F12 - Vollbildmodus an/aus",
                    "Pfeiltaste links/rechts - Selektion Kontrollpunkt",
                    "Pfeiltaste unten/oben - Selektierter Kontrollpunkt heben/senken",
                    "ESC/q/Q - Ende",
                    "o,O/p,P - Flächen reduzieren/hinzufuegen",
                    "+/- - Weichheit der Interpolierten Flaeche",
                    "h/H - Hilfe an/aus",
                    "t/T - Texturierung an/aus",
                    "z/Z - Wechseln der Texturen",
                    "b/B - Pausieren",
                    "i,I/j,J/k,K/l,L - Rotation der Kamera",
                    "w,W/a,A/s,S/d,D - Bewegung der Kamera",
                    "n,N/m,M - rein-/rauszoomen",
                    "x/X | y/Y - Schwarzes Loch hinzufuegen/entfernen"};

    drawString(0.3f, 0.04f, color, help[0]);

    for (GLint i = 1; i < size; ++i) {
        drawString(0.1f, 0.025f + i * 0.04f, color, help[i]);
    }
}

/** Fuellt das Index-Array, welches die Zeichnreihenfolge der Vertices bestimmt
 * @param amountIndices Anzahl der Indizees, die zum zeichnen des Mesh noetig sind
 */
void fillIndexArray(GLint amountIndices) {
    GLint countUpperTri = 0;
    GLint countLowerTri = 0;
    GLint rowCount = -1;
    GLboolean upperTri = GL_FALSE;
    GLint vertsPerSide = getAmountSplineVertices();
    g_splineIndices = malloc(sizeof(GLuint) * amountIndices);
    if (g_splineIndices == NULL) {
        exit(1);
    }
    for (GLint i = 0; i < amountIndices; i += 3) {
        //Jede Reihe besteht aus einer Reihe normaer Dreiecke und einer Reihe die auf dem Kopf steht
        if (upperTri && (countUpperTri % (vertsPerSide - 1) == 0)) {
            upperTri = GL_FALSE;
        } else if (!upperTri && (countLowerTri % (vertsPerSide - 1) == 0)) {
            upperTri = GL_TRUE;
            rowCount++;
        }

        if (upperTri) {
            g_splineIndices[i + 2] = countUpperTri + rowCount;
            g_splineIndices[i + 1] = countUpperTri + vertsPerSide + rowCount;
            g_splineIndices[i] = countUpperTri + 1 + rowCount;
            countUpperTri++;
        } else {
            g_splineIndices[i + 2] = countLowerTri + 1 + rowCount;
            g_splineIndices[i + 1] = countLowerTri + vertsPerSide + rowCount;
            g_splineIndices[i] = countLowerTri + vertsPerSide + 1 + rowCount;
            countLowerTri++;
        }
    }
}

void updateIndexArray(GLint amountIndices) {
    //Neuberechnung der Indizes
    g_splineIndices = realloc(g_splineIndices, sizeof(GLuint) * amountIndices);
    fillIndexArray(amountIndices);
}

/**
 * Aktualisiert das Vertex-Array beim Verandern der Aufloesung des Mesh
 */
void updateScene(void) {
    g_splineSurface = getSplineVertices();
    g_controlVertices = getControlVertices();
    //Vertex und Color Array definieren und bei Änderungen aktualisieren
    glVertexPointer(3,                     //Komponenten pro Vertex (x,y,z)
                    GL_FLOAT,              //Typ der Komponenten
                    sizeof(splineVertex),        //Offset zwischen 2 Vertizes im Array
                    &(g_splineSurface[0][CX])); //Zeiger auf die 1. Komponente
    glNormalPointer(GL_FLOAT, sizeof(splineVertex), &(g_splineSurface[0][CNX]));
    glTexCoordPointer(2,
                      GL_FLOAT,
                      sizeof(splineVertex),
                      &(g_splineSurface[0][CTX]));
    glColorPointer(3, GL_FLOAT, sizeof(splineVertex), &(g_splineSurface[0][CR]));
}


void freeAllocatedMem(void) {
    free(g_controlVertices);
    free(g_splineSurface);
    free(g_splineIndices);
}

/**
 * Zeichnet eine Linie mit der uebergebenen Farbe,
 * mit der Laenge 1 entlang der z-Achse
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawLine(GLint i) {
    glBegin(GL_LINES);
    {
        glColor3f(1, 1, 1);
        glVertex3f(g_splineSurface[i][CNX], g_splineSurface[i][CNY], g_splineSurface[i][CNZ]);
        glVertex3f(0.0f, 0.0f, 0.0f);
    }
    glEnd();
}

/**
 * Zeichnet eine Insel in Form eines Zylinders
 */

/**
 * Zeichnet eine Normale von einem Punkt im Vertex-Array
 * @param i Index des Punktes von dem die Normale gezeichnet wird
 * @param color Farbe, in der die Normale gezeichnet wird
 */
static void drawNormal(GLint i) {
    glPushMatrix();
    {
        glTranslatef(g_splineSurface[i][CX], g_splineSurface[i][CY], g_splineSurface[i][CZ]);
        glScalef(1.0f / 10, 1.0f / 10, 1.0f / 10);
        drawLine(i);
    }
    glPopMatrix();
}

/**
 * Zeichnet alle Kugeln an den entsprechenden Punkten basierend auf dem Vertex-Array
 */
static void drawAllSpheres(void) {
    glPushMatrix();
    {
        glDisable(GL_TEXTURE_2D);
        for (int i = 0; i < SQUARE(getAmountVerticesSide()); i++) {
            glPushMatrix();
            {

                glTranslatef(g_controlVertices[i][CX], g_controlVertices[i][CY], g_controlVertices[i][CZ]);
                glScalef(1.0f / 20, 1.0f / 20, 1.0f / 20);
                const CGColor3f color = {g_controlVertices[i][CR], g_controlVertices[i][CG], g_controlVertices[i][CB]};
                glPushName((GLuint) i);
                {
                    drawSphere(color);
                }
                glPopName();
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet alle Kugeln an den entsprechenden Punkten basierend auf dem Vertex-Array
 */
static void drawAllInterpolSpheres(void) {
    glPushMatrix();
    {
        glDisable(GL_TEXTURE_2D);
        for (int i = 0; i < SQUARE(getAmountSplineVertices()); i++) {
            glPushMatrix();
            {

                glTranslatef(g_splineSurface[i][CX], g_splineSurface[i][CY], g_splineSurface[i][CZ]);
                glScalef(1.0f / 30, 1.0f / 30, 1.0f / 30);
                glPushName((GLuint) i);
                {
                    drawSphere(HIGH_RED);
                }
                glPopName();
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet den Win-Screen
 */
void drawWin() {
    GLfloat color[3] = {1.0f, 1.0f, 0.8f};

    char *help[] = {"GEWONNEN!"};

    drawString(0.5f, 0.5f, color, help[0]);
}

void toggleWin(GLboolean v) {
    showWin = v;
}

/**
 * Zeichnet den Loss-Screen
 */
void drawLoss() {
    GLfloat color[3] = {1.0f, 1.0f, 0.8f};

    char *help[] = {"Verloren! Neustart F9"};

    drawString(0.5f, 0.5f, color, help[0]);
}

void toggleLoss(GLboolean v) {
    showLoss = v;
}

/**
 * Zeichen-Funktion, stellt die Szene dar
 */
void drawScene(void) {
    if (showHelp) {
        drawHelp();
    } else if (showWin) {
        drawWin();
    } else if (showLoss) {
        drawLoss();
    } else {
        GLint amountSplines = getAmountSplineVertices();
        //Punkte aktualisieren
        updateColorSelectedControlVert();
        /* Punktlichtquelle, die die Szene von oben beleuchtet */
        CGPoint4f lightPos0 = {0.0f, 1.5f, 0.0f, 0.0f};

        /* Spotlichtquelle des Leuchtturms, entweder rotierend oder auf ein Boot gerichtet */
        CGVector3f lightDirection0 = {0, -3.0f, 0};

        drawBalls();

        if (getLightingState()) {
            /* Lichtberechnung aktivieren */
            glEnable(GL_LIGHTING);
            /* Rotationswinkel der Lichtquelle */
            GLfloat light0Rotation;
            glPushMatrix();
            {
                /* Richtung der zweiten Lichtquelle aus Rotationswinkel berechnen */
                light0Rotation = getLight1Rotation();
                lightPos0[0] = cosf(DEG2RAD(light0Rotation));
                lightPos0[2] = sinf(DEG2RAD(light0Rotation));
                lightDirection0[0] = -lightPos0[0];
                lightDirection0[2] = -lightPos0[2];
            }
            glPopMatrix();

            if (getLight0State()) {
                /* Position der ersten Lichtquelle setzen */
                glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
                glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection0);
                /* Erste Lichtquelle aktivieren */
                glEnable(GL_LIGHT0);
            }
        }


        if (getTexturingStatus()) {
            /* Texturierung aktivieren */
            glEnable(GL_TEXTURE_2D);
            glDisableClientState(GL_COLOR_ARRAY);
        } else {
            glEnableClientState(GL_COLOR_ARRAY);
        }

        glEnable(GL_COLOR_MATERIAL);
        bindTexture(usedTexture);
        /* Wuerfel */
        glPushMatrix();
        {
            glColor3f(1, 1, 1);
            //Zeichnen (in der drawScene für jeden Frame)
            glDrawElements(GL_TRIANGLES,                             //Primitivtyp
                           SQUARE(amountSplines - 1) * 2 * 3, //Anzahl Indizes zum Zeichnen
                           GL_UNSIGNED_INT,                          //Typ der Indizes
                           g_splineIndices);                   //Index Array
        }
        glPopMatrix();

        if (showSpheres) {
            drawAllSpheres();
        }

        if (showInterpolSpheres) {
            drawAllInterpolSpheres();
        }
        //Sonne zeichnen
        glPushMatrix();
        {
            glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]);
            glScalef(0.2f, 0.2f, 0.2f);
            glColor3f(1.0f, 1.0f, 1.0f);
            glDisable(GL_TEXTURE_2D);
            drawSphere(COLOR_YELLOW);
        }
        glPopMatrix();

        //Falls gewuenscht Normalen zeichnen
        glPushMatrix();
        {
            if (getShowNormal()) {
                for (int i = 0; i < SQUARE(amountSplines); i++) {
                    glPushMatrix();
                    {
                        drawNormal(i);
                    }
                    glPopMatrix();
                }
            }
        }
        glPopMatrix();

        /* Ziel-Sphere zeichnen */
        glPushMatrix();
        {
            goalSphere[CX] = (*getGoalSphere())[CX];
            goalSphere[CY] = (*getGoalSphere())[CY];
            goalSphere[CZ] = (*getGoalSphere())[CZ];
            glTranslatef(goalSphere[CX], goalSphere[CY], goalSphere[CZ]);
            glScalef(GOAL_RADIUS, GOAL_RADIUS, GOAL_RADIUS);
            drawTransparentSphere(COLOR_TRANS_GREEN);
        }
        glPopMatrix();

        /* Schwarze Loecher zeichnen */
        glPushMatrix();
        {
            drawBlackHoles();
        }
        glPopMatrix();

        /* Erste Lichtquelle deaktivieren */
        glDisable(GL_LIGHT0);
        /* Zweite Lichtquelle deaktivieren */
        glDisable(GL_LIGHT1);
        /* Lichtberechnung deaktivieren */
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_COLOR_MATERIAL);

        glPopMatrix();

        /* DEBUG-Ausgabe */
        /*   GLSTATE; */
    }

}

/**
 * Initialisierung der Lichtquellen.
 * Setzt Eigenschaften der Lichtquellen (Farbe, Oeffnungswinkel, ...)
 */
static void
initLight(void) {
    /* Farbe der ersten Lichtquelle */
    CGColor4f lightColor0[3] =
            {{0.2f, 0.2f, 0.2f, 0.5f},
             {0.3f, 0.3f, 0.3f, 0.5f},
             {0.3f, 0.3f, 0.3f, 0.5f}};


    /* Erste Lichtquelle aktivieren */
    glEnable(GL_LIGHT0);

    /* Farbe der ersten Lichtquelle setzen */
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor0[0]);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0[1]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0[2]);
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void) {
    /* Setzen der Farbattribute */
    /* Hintergrundfarbe */
    glClearColor(0.6f, 0.6f, 0.6f, 0.5f);
    /* Zeichenfarbe */
    glColor3f(1.0f, 1.0f, 1.0f);

    /* Z-Buffer-Test aktivieren */
    glEnable(GL_DEPTH_TEST);

    /* Polygonrueckseiten nicht anzeigen */
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    /* Startposition der Kamera festlegen */
    cameraRadiussph = CAMERA_DEFAULT_RADIUS;
    cameraThetaSph = CAMERA_DEFAULT_THETA;
    cameraPhiSph = CAMERA_DEFAULT_PHI;

    //einmalig Vertex und Color Array aktivieren
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    initLight();
    updateScene();
    glLineWidth(1.0f);

    goalSphere[CX] = (*getGoalSphere())[CX];
    goalSphere[CY] = (*getGoalSphere())[CY];
    goalSphere[CZ] = (*getGoalSphere())[CZ];
    /* Alles in Ordnung? */
    return (GLGETERROR == GL_NO_ERROR);
}

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void) {
    /* Flag: Wireframe: ja/nein */
    static GLboolean wireframe = GL_FALSE;

    /* Modus wechseln */
    wireframe = !wireframe;

    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


/**
 * (De-)aktiviert die Anzeige der Normalen.
 */
void toggleNormal(void) {
    setShowNormal(!getShowNormal());
}

/**
 * (De-)aktiviert die Anzeige der Kugeln.
 */
void toggleSpheres(void) {
    showSpheres = !showSpheres;
}

/**
 * (De-)aktiviert die Anzeige der interpolierten Kugeln.
 */
void toggleInterpolSpheres(void) {
    showInterpolSpheres = !showInterpolSpheres;
}

/**
 * Setzt den Kamera-Radius im KugelKoord.
 * Berechnet die Koord im kart. neu
 * 
 * @param[in] rad, Radius
 */
void setCameraRadius(GLfloat rad) {
    cameraRadiussph = rad;
}

/**
 * Setzt den Kamera-Theta Winkel im KugelKoord.
 * Berechnet die Koord im kart. neu
 * 
 * @param[in] theta, winkel
 */
void setCameraTheta(GLfloat theta) {
    cameraThetaSph = theta;
}

/**
 * Setzt den Kamera-Phi Winkel im KugelKoord.
 * Berechnet die Koord im kart. neu
 * 
 * @param[in] phi, winkel
 */
void setCameraPhi(GLfloat phi) {
    cameraPhiSph = phi;
}

/**
 * Liefert den Kamera-Phi Winkel im KugelKoord.
 * 
 * @return phi
 */
GLfloat getCameraPhi(void) {
    return cameraPhiSph;
}

/**
 * Liefert den Kamera-Theta Winkel im KugelKoord.
 * 
 * @return theta
 */
GLfloat getCameraTheta(void) {
    return cameraThetaSph;
}

/**
 * Liefert den Kamera-Radius im KugelKoord.
 * 
 * @return radius
 */
GLfloat getCameraRadius(void) {
    return cameraRadiussph;
}

void toggleShowHelp(void) {
    showHelp = !showHelp;
}

void nextTexture() {
    usedTexture++;
    if (usedTexture > TEX_COUNT - 1) {
        usedTexture = 0;
    }
}