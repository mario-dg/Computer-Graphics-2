/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * @author Mario da Graca, Christopher Ploog
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
#include "spline_surface.h"
#include <math.h>

/**Kameraposition im KugelKoordinatensystem */
static GLfloat cameraRadiussph = 0.0f;
static GLfloat cameraThetaSph = 0.0f;
static GLfloat cameraPhiSph = 0.0f;

static GLint g_amountVerticesSide;
static GLint g_amountSplineVertices;
static controlVertex *g_controlVertices;
static splineVertex *g_splineSurface;

static GLuint *g_splineIndices;

static GLint selectedVert = 0;

static GLint usedTexture = 0;

/** Startposition der Kamera */
#define CAMERA_DEFAULT_RADIUS 3.0f
#define CAMERA_DEFAULT_THETA M_PI_4
#define CAMERA_DEFAULT_PHI M_PI_2

#define LOWER_BORDER (0.15f)
#define MIDDLE_BORDER (0.3f)
#define UPPER_BORDER (0.45f)

#define MIN_SPLINE_AMOUNT (2)
#define DEFAULT_SPLINE_AMOUNT (2)
#define MAX_SPLINE_AMOUNT (60)

#ifndef DEG2RAD
/** Winkelumrechnung von Grad nach Radiant */
#define DEG2RAD(x) ((x) / 180.0f * M_PI)
#endif

/* Umschalten einiger Funktionen */
GLboolean showHelp = GL_FALSE;
GLboolean showSpheres = GL_TRUE;
GLboolean showInterpolSpheres = GL_FALSE;


/* Konstanten fuer Farben */
static const CGColor3f LOW_DARK_BLUE = {0.15f, 0.15f, 0.44f};
static const CGColor3f MEDIUM_GREEN = {0.0f, 1.0f, 0.0f};
static const CGColor3f HIGH_RED = {1.0f, 0.0f, 0.0f};
static const CGColor3f COLOR_YELLOW = {1.0f, 1.0f, 0.0f};
static const CGColor3f COLOR_WHITE = {1.0f, 1.0f, 1.0f};

/**
* Gibt den Hilfetext aus.
*/
static void drawHelp() {
    int size = 20;

    GLfloat color[3] = {1.0f, 0.2f, 0.8f};

    char *help[] = {"Hilfe",
                    "F1 - Wireframe an/aus",
                    "F2 - Normalen an/aus",
                    "F3 - Beleuchtungsberechnung an/aus",
                    "F4 - Punktlichtquelle an/aus",
                    "F6 - Kontroll-Sphaeren an/aus",
                    "F7 - Interpolierte-Sphaeren an/aus",
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
                    "n,N/m,M - rein-/rauszoomen"};

    drawString(0.3f, 0.04f, color, help[0]);

    for (GLint i = 1; i < size; ++i) {
        drawString(0.1f, 0.02f + i * 0.045f, color, help[i]);
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
    GLint vertsPerSide = g_amountSplineVertices;
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

void updateSelectedVert(int i) {
    selectedVert = selectedVert + i;
    if (selectedVert >= SQUARE(g_amountVerticesSide)) {
        selectedVert = 0;
    } else if (selectedVert < 0) {
        selectedVert = SQUARE(g_amountVerticesSide) - 1;
    }
}


/**
 * Aktualisiert die Farben aller Kugeln. 
 * Die Farben sind vom Hoehenwert der jeweiligen Kugel abghaengig
 */
void updateColorSelectedControlVert() {
    CGColor3f color = {1.0f, 1.0f, 1.0f};
    for (GLint i = 0; i < SQUARE(g_amountVerticesSide); i++) {
        //Farbe fuer ausgewaehlten Vertex
        if (i == selectedVert) {
            color[0] = COLOR_WHITE[2];
            color[1] = COLOR_WHITE[2];
            color[2] = COLOR_WHITE[2];
        } else {
            //Farbe fuer nicht ausgewaehlten Vertex
            color[0] = LOW_DARK_BLUE[0];
            color[1] = LOW_DARK_BLUE[1];
            color[2] = LOW_DARK_BLUE[2];
        }

        g_controlVertices[i][CR] = color[0];
        g_controlVertices[i][CG] = color[1];
        g_controlVertices[i][CB] = color[2];
    }
}


/**
 * Berechnung der Polynome
 * @param mode Art der Berechnung
 *             interpolation -> Berechnung der Splineflaeche
 *             derivative_s -> Partielle Ableitung in s
 *             derivative_t -> Partielle Ableitung in t
 * @param v höherwertige Berechnung (Komponenten seperat)
 *          CX -> X-Werte
 *          CY -> Y-Werte
 *          CZ -> Z-Werte
  * @return GLfloat* mit allen berechneten Komponenten
 */
static GLfloat *calculateSurfaceValues(calculationMode mode, GLint v) {
    GLfloat *temp = malloc(sizeof(GLfloat) * SQUARE(g_amountVerticesSide));
    for (int j = 0; j < SQUARE(g_amountVerticesSide); ++j) {
        temp[j] = g_controlVertices[j][v];
    }
    GLfloat *result = splineSurface(g_amountVerticesSide, g_amountSplineVertices, temp, mode);

    free(temp);

    return result;
}

/**
 * Berechnet das Kreuzprodukt zweier uebergebener Vektoren
 * @param a Vektor A
 * @param b Vektor B
 * @param result Ergebnis
 */
static void crossProduct(const CGVector3f a, const CGVector3f b, CGVector3f result) {
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];

    //Normalisieren der Laenge der Normalen
    GLfloat len = sqrt(result[0] * result[0] + result[1] * result[1] + result[2] * result[2]);
    result[0] /= len;
    result[1] /= len;
    result[2] /= len;
}

/**
 * Berechnung der Normalen der interpolierten Splinepunkte
 * @return CGVector3f* Vektor-Pointer mit allen Normalen
 */
static CGVector3f *calculateNormals() {
    //Partielle Ableitung der Polynome in s
    GLfloat *vsx = calculateSurfaceValues(derivative_s, CX);
    GLfloat *vsy = calculateSurfaceValues(derivative_s, CY);
    GLfloat *vsz = calculateSurfaceValues(derivative_s, CZ);

    //Partielle Ableitung der Polynome in t
    GLfloat *vtx = calculateSurfaceValues(derivative_t, CX);
    GLfloat *vty = calculateSurfaceValues(derivative_t, CY);
    GLfloat *vtz = calculateSurfaceValues(derivative_t, CZ);

    CGVector3f *result = malloc(sizeof(CGVector3f) * SQUARE(g_amountSplineVertices));

    for (int j = 0; j < SQUARE(g_amountSplineVertices); ++j) {
        //Kreuzprodukt der partiellen Ableitung bilden
        CGVector3f a;
        a[0] = vsx[j];
        a[1] = vsy[j];
        a[2] = vsz[j];
        CGVector3f b;
        b[0] = vtx[j];
        b[1] = vty[j];
        b[2] = vtz[j];
        CGVector3f temp;
        crossProduct(a, b, temp);
        result[j][0] = temp[0];
        result[j][1] = temp[1];
        result[j][2] = temp[2];
    }
    //Reservierten Speicherplatz freigeben
    free(vsx);
    free(vsy);
    free(vsz);
    free(vtx);
    free(vty);
    free(vtz);
    return result;
}

/**
 * Zuweisung der Farbe eines Vertizes anhand der Hoehe
 */
static void calculateColors() {
    CGColor3f temp = {1.0f, 1.0f, 1.0f};
    for (int i = 0; i < SQUARE(g_amountSplineVertices); ++i) {
        //Farbe fuer niedrige Hoehen
        if (g_splineSurface[i][CY] < LOWER_BORDER) {
            temp[0] = LOW_DARK_BLUE[0];
            temp[1] = LOW_DARK_BLUE[1];
            temp[2] = LOW_DARK_BLUE[2];
        }
            //Farbe fuer mittlere Hoehen
        else if (g_splineSurface[i][CY] < MIDDLE_BORDER) {
            temp[0] = COLOR_YELLOW[0];
            temp[1] = COLOR_YELLOW[1];
            temp[2] = COLOR_YELLOW[2];
        }
            //Farbe fuer mittlere Hoehen
        else if (g_splineSurface[i][CY] < UPPER_BORDER) {
            temp[0] = MEDIUM_GREEN[0];
            temp[1] = MEDIUM_GREEN[1];
            temp[2] = MEDIUM_GREEN[2];
        } else
            //Farbe fuer groesste Hoehen
        {
            temp[0] = HIGH_RED[0];
            temp[1] = HIGH_RED[1];
            temp[2] = HIGH_RED[2];
        }
        g_splineSurface[i][CR] = temp[0];
        g_splineSurface[i][CG] = temp[1];
        g_splineSurface[i][CB] = temp[2];
    }
}

/**
 * Updated das Vertex-Array fuer die interpolierte Splineflaeche
 * @param totalAmntSplineVertices
 */
void updateSplineVertexArray(GLint totalAmntSplineVertices) {
    //X, Y und Z Werte der interpolierten Splineflaeche
    GLfloat *splineX = calculateSurfaceValues(interpolation, CX);
    GLfloat *splineY = calculateSurfaceValues(interpolation, CY);
    GLfloat *splineZ = calculateSurfaceValues(interpolation, CZ);

    //Normalen der interpolierten Splinepunkte
    CGVector3f *normals = calculateNormals();

    GLfloat currTexX = 0.0f;
    GLfloat currTexY = -1.0f / (float) (g_amountSplineVertices - 1);

    for (int j = 0; j < totalAmntSplineVertices; ++j) {
        if (j % g_amountSplineVertices == 0) {
            currTexX = 0.0f;
            currTexY += 1.0f / (float) (g_amountSplineVertices - 1);
        } else {
            currTexX += 1.0f / (float) (g_amountSplineVertices - 1);
        }
        g_splineSurface[j][CX] = splineX[j];
        g_splineSurface[j][CY] = splineY[j];
        g_splineSurface[j][CZ] = splineZ[j];
        g_splineSurface[j][CNX] = normals[j][0];
        g_splineSurface[j][CNY] = normals[j][1];
        g_splineSurface[j][CNZ] = normals[j][2];
        g_splineSurface[j][CTX] = currTexX;
        g_splineSurface[j][CTY] = currTexY;
    }
    //Bei keiner Texturierung -> Faerbung der Vertizes abhaengig von der Hoehe
    calculateColors();

    //Speicher wieder freigeben
    free(splineX);
    free(splineY);
    free(splineZ);
    free(normals);
}

/**
 * Initialisiert das Vertex-Array beim Programmstart
 */
void initVertexArray(void) {
    // g_amountVerticesSide * g_amountVerticesSide Quadrate im Mesh
    // 2 Dreiecke pro Quadrat
    // 3 Vertices pro Dreieck
    GLint amountIndices = SQUARE(g_amountSplineVertices - 1) * 2 * 3;
    GLint totalAmntSplineVertices = SQUARE(g_amountSplineVertices);
    GLfloat currX = -1.0f;
    GLfloat currZ = -1.0f - (2.0f / (g_amountVerticesSide - 1));
    GLfloat *heights = getHeights();

    g_controlVertices = malloc(sizeof(controlVertex) * SQUARE((g_amountVerticesSide)));
    if (g_controlVertices == NULL) {
        exit(1);
    } else {
        g_splineIndices = malloc(sizeof(GLuint) * amountIndices);
        if (g_splineIndices == NULL) {
            free(g_controlVertices);
            exit(1);
        }
    }

    g_splineSurface = malloc(sizeof(splineVertex) * totalAmntSplineVertices);
    if (g_splineSurface == NULL) {
        free(g_controlVertices);
        free(g_splineIndices);
        exit(1);
    }

    fillIndexArray(amountIndices);

    for (GLint i = 0; i < SQUARE(g_amountVerticesSide); i++) {
        if (i % g_amountVerticesSide == 0) {
            currX = -1.0f;
            currZ += 2.0f / (g_amountVerticesSide - 1);
        } else {
            currX += 2.0f / (g_amountVerticesSide - 1);
        }
        g_controlVertices[i][CX] = currX;
        g_controlVertices[i][CY] = heights[i];
        g_controlVertices[i][CZ] = currZ;
    }
    updateSplineVertexArray(totalAmntSplineVertices);
}

/**
 * Aktualisiert das Vertex-Array beim Verandern der Aufloesung des Mesh
 */
void updateScene(void) {
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

void updateVertexArray(expandShrinkVertices state) {
    GLfloat currX = -1.0f;
    GLfloat currZ;
    GLfloat *newHeights;
    g_amountVerticesSide += state;
    //neue Hoehen aus der Logik holen
    newHeights = calloc(SQUARE(g_amountVerticesSide), sizeof(GLfloat));
    if (newHeights != NULL) {
        for (GLint i = 0; i < SQUARE(g_amountVerticesSide); i++) {
            newHeights[i] = getHeights()[i];
        }

        currZ = -1.0f - (2.0f / (g_amountVerticesSide - 1));

        g_controlVertices = realloc(g_controlVertices, sizeof(controlVertex) * SQUARE(g_amountVerticesSide));
        if (g_controlVertices == NULL) {
            exit(1);
        } else {
            for (GLint i = 0; i < SQUARE(g_amountVerticesSide); i++) {
                if (i % g_amountVerticesSide == 0) {
                    currX = -1.0f;
                    currZ += 2.0f / (g_amountVerticesSide - 1);
                } else {
                    currX += 2.0f / (g_amountVerticesSide - 1);
                }
                g_controlVertices[i][CX] = currX;
                g_controlVertices[i][CY] = newHeights[i];
                g_controlVertices[i][CZ] = currZ;
            }
            updateSplineVertexArray(SQUARE(g_amountSplineVertices));
            updateScene();
        }
    } else {
        exit(1);
    }
}

/**
 * Aktualisiert die Hoehen, indem die Werte aus der Logik uebernommen werden
 */
void updateHeights(void) {
    for (GLint i = 0; i < SQUARE(g_amountVerticesSide); i++) {
        g_controlVertices[i][CY] = getHeights()[i];
    }
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
        for (int i = 0; i < SQUARE(g_amountVerticesSide); i++) {
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
        for (int i = 0; i < SQUARE(g_amountSplineVertices); i++) {
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
 * Zeichen-Funktion, stellt die Szene dar
 */
void drawScene(void) {
    if (showHelp) {
        drawHelp();
    } else {
        //Punkte aktualisieren
        updateHeights();
        updateColorSelectedControlVert();
        /* Punktlichtquelle, die die Szene von oben beleuchtet */
        CGPoint4f lightPos0 = {0.0f, 1.5f, 0.0f, 0.0f};

        /* Spotlichtquelle des Leuchtturms, entweder rotierend oder auf ein Boot gerichtet */
        CGVector3f lightDirection0 = {0, -3.0f, 0};


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
                           SQUARE(g_amountSplineVertices - 1) * 2 * 3, //Anzahl Indizes zum Zeichnen
                           GL_UNSIGNED_INT,                          //Typ der Indizes
                           g_splineIndices);                   //Index Array
        }
        glPopMatrix();

        if (showSpheres) {
            drawAllSpheres();
        }

        if(showInterpolSpheres){
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

        glPushMatrix();
        {
            if (getShowNormal()) {
                for (int i = 0; i < SQUARE(g_amountSplineVertices); i++) {
                    glPushMatrix();
                    {
                        drawNormal(i);
                    }
                    glPopMatrix();
                }
            }
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

    g_amountVerticesSide = START_AMOUNT_VERTICES;
    g_amountSplineVertices = DEFAULT_SPLINE_AMOUNT;
    initLight();
    initVertexArray();
    updateScene();

    glLineWidth(1.0f);

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

void updateSelectedHeight(int direction) {
    pickedVertex(selectedVert, direction);
    updateVertexArray(0);
    updateSplineVertexArray(SQUARE(g_amountSplineVertices));
    updateScene();
}

void updateSplineVertices(GLint v) {
    if (g_amountSplineVertices + v >= MIN_SPLINE_AMOUNT
        && g_amountSplineVertices + v <= MAX_SPLINE_AMOUNT) {
        g_amountSplineVertices += v;

        //Neuberechnung der interpolierten Punkte
        g_splineSurface = realloc(g_splineSurface, sizeof(splineVertex) * SQUARE(g_amountSplineVertices));
        updateSplineVertexArray(SQUARE(g_amountSplineVertices));

        //Neuberechnung der Indizes
        g_splineIndices = realloc(g_splineIndices, sizeof(GLuint) * SQUARE(g_amountSplineVertices - 1) * 2 * 3);
        fillIndexArray(SQUARE(g_amountSplineVertices - 1) * 2 * 3);
        updateScene();
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

GLint getAmountVertices(void) {
    return g_amountVerticesSide;
}

void toggleShowHelp(void) {
    showHelp = !showHelp;
}

void nextTexture(){
    usedTexture++;
    if(usedTexture > TEX_COUNT - 1){
        usedTexture = 0;
    }
}