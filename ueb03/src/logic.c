/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Wesentliche Programmlogik ist die
 * Verwaltung und Aktualisierung der Position und Bewegung eines Rechtecks. Die
 * Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Christopher Ploog, Mario da Graca
 */

/* ---- Standard Header einbinden ---- */
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "scene.h"
#include "spline_surface.h"
#include "euler_integration.h"

/** Anzahl der Drehungen des rotierenden Lichtes pro Sekunde */
#define LIGHT1_ROTATIONS_PS 0.25f

#define PICK_HEIGHT (0.25f)

#define MAX_HEIGHT (0.25f)

#define DEFAULT_SLOPE (0.1f)

#define KICK_STRENGTH (1.0f)

#define RANDOM_HEIGHT (((float) rand() / (float) RAND_MAX) * MAX_HEIGHT);

#define RANDOM_KICK (((float) rand() / (float) RAND_MAX) * KICK_STRENGTH);

#define RANDOM_IN_RANGE(max) ((((float) rand() / (float) RAND_MAX) * (max)) - ((max) / 2.0f))

#define CALC_BALL_RADIUS(width) (((width) / 3.0f / (g_amountBalls)))

#define CALC_INNER_BH_RADIUS(width) (((width) / 2.0f / (g_amountBalls)))
#define CALC_OUTER_BH_RADIUS(width) (((width) / 1.0f / (g_amountBalls)))
/* ---- Konstanten ---- */

/** Status der der Lichtberechnung (an/aus) */
static GLboolean g_lightingState = GL_TRUE;

/** Status der ersten Lichtquelle (an/aus) */
static GLboolean g_light0State = 1;

/** Status der zweiten Lichtquelle (rotierend/aus/boot1/boot2) */
static light1State g_light1State = 0;

/** Rotationswinkel der zweiten Lichtquelle. */
static float g_light1RotationAngle = 0.0f;

/** Selektierter Kontrollpunkt */
static GLint selectedVert = 0;

/** Kontroll und Spline Vertizes */
static controlVertex *g_controlVertices;
static splineVertex *g_splineSurface;

/** Anzahl Kontroll und Spline Vertizes */
static GLint g_amountVerticesSide;
static GLint g_amountSplineVertices;

/** Alle Baelle die ueber die Flaeche rollen */
static ball *g_allBalls;
/** Anzahl der Baelle */
static GLint g_amountBalls;

/** Koordinatenwerte der Raender der Flaeche */
static GLfloat g_upperBorder, g_lowerBorder, g_leftBorder, g_rightBorder;

/** Rollen der Baelle aktieviere */
static GLboolean startBalls = GL_FALSE;

/** Radius der Baelle */
static GLfloat ballRadius = DEFAULT_BALL_RADIUS;

/** Speichert den ersten Ball der noch im Spiel ist */
static GLuint firstInPlayBall = 0;

/** Koordinaten der Zielkugel */
static CGPoint3f g_goalSphere;

/** Alle schwarzen Loecher */
static blackHole *g_allBlackholes;
/** Anzahl der schwarzen Loecher */
static GLint g_amountBlackHoles;

/** Aktiviert die Moeglichkeit zu gewinnen
 *  Da Wandstuecke nicht implementiert -> Kann man die Animation trotzdem genauer testen
 */
static GLboolean g_enableWinning;

/**
 * Liefert alle x/y/z Koordinaten der Kontrollvertizes
 * @param v CX/CY/CZ
 * @return Zeiger auf alle gesuchten Koordinaten
 */
GLfloat *getCoords(GLint v) {
    GLfloat *temp = malloc(sizeof(GLfloat) * SQUARE(g_amountVerticesSide));
    for (int j = 0; j < SQUARE(g_amountVerticesSide); ++j) {
        temp[j] = g_controlVertices[j][v];
    }
    return temp;
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
    GLfloat *temp = getCoords(v);
    GLfloat *result = splineSurface(g_amountVerticesSide, g_amountSplineVertices, temp, mode);

    free(temp);

    return result;
}

/**
 * Berechnet die Koordinaten der uebergebenen x und z Werte auf der Splineflaeche
 * @param x x-Wert
 * @param z z-Wert
 * @param mode Art der Berechnung
 *             interpolation -> Berechnung der Splineflaeche
 *             derivative_s -> Partielle Ableitung in s
 *             derivative_t -> Partielle Ableitung in t
 * @param v höherwertige Berechnung (Komponenten seperat)
 *          CX -> X-Werte
 *          CY -> Y-Werte
 *          CZ -> Z-Werte
 * @return GLfloat Interpolierter Wert
 */
static GLfloat calcInterpolatedCoord(GLfloat x, GLfloat z, calculationMode mode, GLint v) {
    //x und z linear auf 0-1 abbilden
    GLfloat bigT = (x + 1.0f) / 2.0f;
    GLfloat bigS = (z + 1.0f) / 2.0f;
    GLfloat t;
    GLfloat s;
    GLint tSection = getSection(bigT, &t, g_amountVerticesSide - 3);
    GLint sSection = getSection(bigS, &s, g_amountVerticesSide - 3);
    GLfloat *values = getCoords(v);
    CGMatrix44f geom;
    matrixMultiplication(values, tSection, sSection, g_amountVerticesSide, geom);
    GLfloat result = calcSplineCoords(s, t, geom, mode);
    free(values);
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
    GLint amountSplines = getAmountSplineVertices();
    //Partielle Ableitung der Polynome in s
    GLfloat *vsx = calculateSurfaceValues(derivative_s, CX);
    GLfloat *vsy = calculateSurfaceValues(derivative_s, CY);
    GLfloat *vsz = calculateSurfaceValues(derivative_s, CZ);

    //Partielle Ableitung der Polynome in t
    GLfloat *vtx = calculateSurfaceValues(derivative_t, CX);
    GLfloat *vty = calculateSurfaceValues(derivative_t, CY);
    GLfloat *vtz = calculateSurfaceValues(derivative_t, CZ);

    CGVector3f *result = malloc(sizeof(CGVector3f) * SQUARE(amountSplines));

    for (int j = 0; j < SQUARE(amountSplines); ++j) {
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
    free(vsx);
    free(vsy);
    free(vsz);
    free(vtx);
    free(vty);
    free(vtz);
    return result;
}

/**
 * Berechnet die Normale des uebergebenen Punktes auf der Splineflaeche
 * @param bigT linear abgebildete x-Koordinate
 * @param bigS linear abgebildete z-Koordinate
 * @param result resultierende Normale
 */
static void calculateNormalsEuler(GLfloat x, GLfloat y, CGVector3f result) {
    // x/y/z-Koordinaten auf der Splineflaeche bestimmen mit abgeleitetem s
    GLfloat vsx = calcInterpolatedCoord(x, y, derivative_s, CX);
    GLfloat vsy = calcInterpolatedCoord(x, y, derivative_s, CY);
    GLfloat vsz = calcInterpolatedCoord(x, y, derivative_s, CZ);

    // x/y/z-Koordinaten auf der Splineflaeche bestimmen mit abgeleitetem t
    GLfloat vtx = calcInterpolatedCoord(x, y, derivative_t, CX);
    GLfloat vty = calcInterpolatedCoord(x, y, derivative_t, CY);
    GLfloat vtz = calcInterpolatedCoord(x, y, derivative_t, CZ);

    //Kreuzprodukt der partiellen Ableitung bilden
    CGVector3f a;
    a[0] = vsx;
    a[1] = vsy;
    a[2] = vsz;
    CGVector3f b;
    b[0] = vtx;
    b[1] = vty;
    b[2] = vtz;
    crossProduct(a, b, result);
}

/**
 * Zuweisung der Farbe eines Vertizes anhand der Hoehe
 */
static void calculateColors() {
    CGColor3f temp = {1.0f, 1.0f, 1.0f};
    for (int i = 0; i < SQUARE(getAmountSplineVertices()); ++i) {
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
    updateScene();

    //Speicher wieder freigeben
    free(splineX);
    free(splineY);
    free(splineZ);
    free(normals);
}

/**
 * Bestimmt die Waende der Flaeche
 * @param upper Obere Grenze
 * @param lower Untere Grenze
 * @param left Linke Grenze
 * @param right Rechte Grenze
 */
void calcBorders(GLfloat *upper, GLfloat *lower, GLfloat *left, GLfloat *right) {
    *upper = g_splineSurface[0][CZ];
    *lower = g_splineSurface[SQUARE(g_amountSplineVertices) - 1][CZ];
    *left = g_splineSurface[0][CX];
    *right = g_splineSurface[SQUARE(g_amountSplineVertices) - 1][CX];
}

/**
 * Koordinaten des Ziels bei vergroesserung des Feldes anpassen
 */
void updateGoal() {
    //z-Wert neu setzen
    g_goalSphere[CZ] = g_lowerBorder - ballRadius;
    //y-Wert neu bestimmen
    g_goalSphere[CY] = calcInterpolatedCoord(g_goalSphere[0], g_goalSphere[2], interpolation, CY);
}


/**
 * Koordinaten des Ziels initialisieren
 */
void initGoal() {
    //x-Wert zufaellig auf der Splineflaeche
    GLfloat temp = RANDOM_IN_RANGE(2.0f);
    g_goalSphere[CX] = temp;

    //y-Wert auf der Splineflaeche bestimmen
    g_goalSphere[CY] = 0.0f;

    //z-Wert an den unteren Rand setzen
    g_goalSphere[CZ] = g_lowerBorder - ballRadius;

    //x-Wert auf der Splineflaeche bestimmen
    g_goalSphere[CX] = calcInterpolatedCoord(g_goalSphere[CX], g_goalSphere[CZ], interpolation, CX);
    //y-Wert auf der Splineflaeche bestimmen
    g_goalSphere[CY] = calcInterpolatedCoord(g_goalSphere[CX], g_goalSphere[CZ], interpolation, CY);
}

/**
 * Aktualisiert die schwarzen Loecher
 * @param v +1/-1
 */
void updateBlackHoleHeight(){
    // ueber alle BH laufen
    for (int i = 0; i < g_amountBlackHoles; ++i) {
        GLfloat x = g_allBlackholes[i].center[0];
        GLfloat z = g_allBlackholes[i].center[2];

        //interpolierte Hoehe berechnen und das BH anpassen
        g_allBlackholes[i].center[CY] = calcInterpolatedCoord(x, z, interpolation, CY);
    }
}

/**
 * Positionen der schwarzen Loecher auf der Splineflaeche bestimmen
 * @param update
 */
void calcInterpolBlackHolePos(GLboolean update) {
    GLint i = 0;
    //Falls update = true nur das letzte BH updaten
    if (update) {
        i = g_amountBlackHoles - 1;
    }
    //Laufen und updaten aller BHs
    for (; i < g_amountBlackHoles; ++i) {
        GLfloat x = g_allBlackholes[i].center[0];
        GLfloat z = g_allBlackholes[i].center[2];

        //Interpolierte Position des BH berechnen
        g_allBlackholes[i].center[CX] = calcInterpolatedCoord(x, z, interpolation, CX);
        g_allBlackholes[i].center[CY] = calcInterpolatedCoord(x, z, interpolation, CY);
        g_allBlackholes[i].center[CZ] = calcInterpolatedCoord(x, z, interpolation, CZ);
    }
}

/**
 * Bestimmt die Koordinaten der Baelle auf der Splineflaeche
 * @param calcX soll x bestimmt werden
 * @param calcY soll y bestimmt werden
 * @param calcZ soll z bestimmt werden
 */
void calcInterpolBallPos(GLboolean calcX, GLboolean calcY, GLboolean calcZ) {
    //ueber alle baelle laufen
    for (int i = 0; i < g_amountBalls; ++i) {
        GLfloat x = g_allBalls[i].center[0];
        GLfloat z = g_allBalls[i].center[2];

        //Falls uebergeben jewalige Koordinate interpolieren
        if (calcX) {
            g_allBalls[i].center[0] = calcInterpolatedCoord(x, z, interpolation, CX);
        }
        if (calcY) {
            g_allBalls[i].center[1] = calcInterpolatedCoord(x, z, interpolation, CY);
        }
        if (calcZ) {
            g_allBalls[i].center[2] = calcInterpolatedCoord(x, z, interpolation, CZ);
        }

    }
}

/**
 * Prueft ob das Spiel gewonnen, verloren oder noch am laufen ist
 * @param currBall
 * @return 0 -> lauft nocht
 *         1 -> gewonnen
 *         2 -> verloren
 */
GLint checkGameWon(ball currBall) {
    //ueberpruefen, ob ein Ball im Zielradius ist
    if (squareDist(currBall.center, g_goalSphere) <= (ballRadius + GOAL_RADIUS)) {
        //Spiel gewonnen
        return 1;
    }
    for (int i = 0; i < g_amountBalls; ++i) {
        if (g_allBalls[i].active == GL_TRUE) {
            //Mindestens ein Ball noch aktiv, Spiel lauft noch
            return 0;
        }
    }
    //Kein aktiver Ball mehr uebrig, Spiel verloren
    return 2;
}

/**
 * Handlet die Animation der und Kollision der Baelle
 * @param interval Zeitintervall zwischen zwei Frames
 */
void calcInterpolBall(double interval) {
    //Bei noch nicht gestartetem Spiel
    if (!startBalls) {
        interval = 0.0f;
    }
    GLint won = 0;

    //Wandpositionen berrechnen
    calcBorders(&g_upperBorder, &g_lowerBorder, &g_leftBorder, &g_rightBorder);
    //Ballposition updaten.
    calcInterpolBallPos(GL_FALSE, GL_TRUE, GL_FALSE);

    //ueber alle Baelle laufen, position berechnen und auf Sieg pruefen
    for (int i = 0; i < g_amountBalls && (won == 0); ++i) {
        //Nur noch im Spiel befindliche Baelle verwenden
        if (g_allBalls[i].active) {
            CGVector3f normal;
            calculateNormalsEuler(g_allBalls[i].center[0], g_allBalls[i].center[2], normal);

            //Kontaktpunkt mit Eulerflaeche berrehcnen
            calcContactPoint(&g_allBalls[i], normal, interval, g_rightBorder, g_leftBorder, g_upperBorder,
                             g_lowerBorder, g_allBlackholes, g_amountBlackHoles,
                             g_allBalls, g_amountBalls);
            //Falls Gewonnen werden kann, ueberpruefen, ob wurde
            if(g_enableWinning) {
                won = checkGameWon(g_allBalls[i]);
            }
        }
    }
    //Falls gewonnen wurde win-screen toggeln
    if (won == 1) {
        toggleWin(GL_TRUE);
    } else if (won == 2) {
        //Falls verloren wurde loss-screen toggeln
        toggleLoss(GL_TRUE);
    }
}

/**
 * Initialisiert das Vertex-Array beim Programmstart
 */
void initVertexArray(void) {
    g_controlVertices = calloc(SQUARE(g_amountVerticesSide), sizeof(controlVertex));
    if (g_controlVertices == NULL) {
        exit(1);
    } else {
        g_splineSurface = calloc(SQUARE(g_amountSplineVertices), sizeof(splineVertex));
        if (g_splineSurface == NULL) {
            free(g_controlVertices);
            exit(1);
        }
    }
    // g_amountVerticesSide * g_amountVerticesSide Quadrate im Mesh
    // 2 Dreiecke pro Quadrat
    // 3 Vertices pro Dreieck
    GLint amountIndices = SQUARE(g_amountSplineVertices - 1) * 2 * 3;
    GLint totalAmntSplineVertices = SQUARE(g_amountSplineVertices);
    GLfloat currX = -1.0f;
    GLfloat currZ = -1.0f - (2.0f / (g_amountVerticesSide - 1));

    g_controlVertices = malloc(sizeof(controlVertex) * SQUARE((g_amountVerticesSide)));
    if (g_controlVertices == NULL) {
        exit(1);
    }

    g_splineSurface = malloc(sizeof(splineVertex) * totalAmntSplineVertices);
    if (g_splineSurface == NULL) {
        free(g_controlVertices);
        exit(1);
    }

    fillIndexArray(amountIndices);
    GLfloat slope = 0.0f;
    GLint count = 0;
    for (GLint i = 0; i < SQUARE(g_amountVerticesSide); i++) {
        if (i % g_amountVerticesSide == 0) {
            currX = -1.0f;
            currZ += 2.0f / (g_amountVerticesSide - 1);
            slope = count++ * DEFAULT_SLOPE;
        } else {
            currX += 2.0f / (g_amountVerticesSide - 1);
        }
        g_controlVertices[i][CX] = currX;
        g_controlVertices[i][CY] = RANDOM_HEIGHT;
        g_controlVertices[i][CY] -= slope;
        g_controlVertices[i][CZ] = currZ;
    }
}


/**
 * Verteilt die Baelle zum Start zufaellig am oberen Rand
 */
void calcStartPositions() {
    GLfloat width = fabsf(g_leftBorder - g_rightBorder);
    ballRadius = CALC_BALL_RADIUS(width);
    //ersten Ball manuel platzieren
    g_allBalls[0].center[CX] = RANDOM_IN_RANGE(width);
    //ueber restliche Baelle itarieren und diese plazieren
    for (int i = 1; i < g_amountBalls; ++i) {
        GLboolean error = GL_TRUE;
        GLfloat x;
        //solange probieren den Ball zu platzieren, bis ein freier Platz gefunden wirde
        do{
            error = GL_TRUE;
            x = RANDOM_IN_RANGE(width);
            //pruefen, ob Position frei ist
            for (int k = 0; k < i && error; ++k) {
                if (fabsf(g_allBalls[k].center[CX] - x) < 2 * ballRadius) {
                    error = GL_FALSE;
                }
            }
        } while (!error);
        g_allBalls[i].center[CX] = x;
    }

}

/**
 * Setzt die Baelle an den Start zurück
 */
void resetGame() {
    //Ball Positionen reseten
    for (int i = 0; i < g_amountBalls; ++i) {
        g_allBalls[i].active = GL_TRUE;
        g_allBalls[i].center[CX] = 0.0f;
        g_allBalls[i].center[CY] = 0.0f;
        g_allBalls[i].center[CZ] = -1.0f + 0.05f;
        g_allBalls[i].acc[CX] = 0.0f;
        g_allBalls[i].acc[CY] = 0.0f;
        g_allBalls[i].acc[CZ] = 0.0f;
        g_allBalls[i].vel[CX] = 0.0f;
        g_allBalls[i].vel[CY] = 0.0f;
        g_allBalls[i].vel[CZ] = 0.0f;
    }
    startBalls = GL_FALSE;

    //Ball Start-Positionen berechnen
    calcStartPositions();
    calcInterpolBallPos(GL_FALSE, GL_TRUE, GL_TRUE);

    GLfloat width = fabsf(g_leftBorder - g_rightBorder);

    //Schwarze Loecher zufaellig platzieren
    for (int i = 0; i < g_amountBlackHoles; ++i) {
        g_allBlackholes[i].center[CX] = RANDOM_IN_RANGE(2.0f);
        g_allBlackholes[i].center[CY] = 0.0f;
        g_allBlackholes[i].center[CZ] = RANDOM_IN_RANGE(2.0f);
        g_allBlackholes[i].innerRadius = CALC_INNER_BH_RADIUS(width);
        g_allBlackholes[i].outerRadius = CALC_OUTER_BH_RADIUS(width);
    }
    calcInterpolBlackHolePos(GL_FALSE);
}

/**
 * Initialisiert die schwarzen Loecher
 */
void initBlackHoles(){
    GLfloat width = fabsf(g_leftBorder - g_rightBorder);
    g_allBlackholes = malloc(g_amountBlackHoles * sizeof(*g_allBlackholes));
    //Schwarze Loecher zufaellig platzieren
    for (int i = 0; i < g_amountBlackHoles; ++i) {
        g_allBlackholes[i].center[CX] = RANDOM_IN_RANGE(2.0f);
        g_allBlackholes[i].center[CY] = 0.0f;
        g_allBlackholes[i].center[CZ] = RANDOM_IN_RANGE(2.0f);
        g_allBlackholes[i].innerRadius = CALC_INNER_BH_RADIUS(width);
        g_allBlackholes[i].outerRadius = CALC_OUTER_BH_RADIUS(width);
    }
    //Schwarze Loecher interpolieren
    calcInterpolBlackHolePos(GL_FALSE);
}

/**
 * Initialisiert alle Baelle
 */
void initBalls(){
    g_allBalls = malloc(g_amountBalls * sizeof(*g_allBalls));

    if (g_allBalls == NULL) {
        free(g_splineSurface);
        free(g_controlVertices);
        exit(1);
    }
    //Baelle initialisieren
    for (int i = 0; i < g_amountBalls; ++i) {
        g_allBalls[i].id = i;
        g_allBalls[i].active = GL_TRUE;
        g_allBalls[i].center[CX] = 0.0f;
        g_allBalls[i].center[CY] = 0.0f;
        g_allBalls[i].center[CZ] = g_upperBorder + 0.05f;
        g_allBalls[i].acc[CX] = 0.0f;
        g_allBalls[i].acc[CY] = 0.0f;
        g_allBalls[i].acc[CZ] = 0.0f;
        g_allBalls[i].vel[CX] = 0.0f;
        g_allBalls[i].vel[CY] = 0.0f;
        g_allBalls[i].vel[CZ] = 0.0f;
    }
    calcStartPositions();
    calcInterpolBallPos(GL_FALSE, GL_TRUE, GL_FALSE);
}

/**
 * Initialisiertt die Logik
 */
void initLogic(void) {
    //Zufallsgenerator setzen
    srand(time(NULL));
    //Anzahl der Kontroll Vertizes auf StartWert setzen
    g_amountVerticesSide = DEFAULT_AMOUNT_VERTICES;
    //Anzahl der interpolierten Spline Vertizes auf Startwert setzen
    g_amountSplineVertices = DEFAULT_SPLINE_AMOUNT;
    //Anzahl der Baelle festlegen
    g_amountBalls = DEFAULT_AMOUNT_BALLS;
    //Anzahl der schwarzen Loecher auf Startwert setzen
    g_amountBlackHoles = DEFAULT_AMOUNT_BH;
    //Startwert fuers gewinnen gesetzt
    g_enableWinning = GL_TRUE;
    //Erste Ball der noch im Spiel ist -> Am Start immer index 0
    firstInPlayBall = 0;
    //Selektierter Kontrollvertex am Start immer index 0
    selectedVert = 0;

    //Kontrollvertex-Array initialisieren
    initVertexArray();

    //Splineflaeche erstellen
    updateSplineVertexArray(SQUARE(g_amountSplineVertices));

    //Grenzen/Waende der Flaeche bestimmen
    calcBorders(&g_upperBorder, &g_lowerBorder, &g_leftBorder, &g_rightBorder);

    //Baelle initialisieren
    initBalls();

    //Zielsphaere initialisieren
    initGoal();

    //schwarze Loecher initialisieren
    initBlackHoles();

    //Daten an Szene senden
    updateScene();
}

/**
 * Updated die Anzahl der schwarzen Loecher. Entfernt das zuletzt hinzugefuegte
 * @param v +1/-1
 */
void updateBlackHoles(GLint v) {
    g_amountBlackHoles += v;
    if (g_amountBlackHoles < MIN_AMOUNT_BH) {
        g_amountBlackHoles = MIN_AMOUNT_BH;
    } else if (g_amountBlackHoles > MAX_AMOUNT_BH) {
        g_amountBlackHoles = MAX_AMOUNT_BH;
    } else {
        GLfloat width = fabsf(g_leftBorder - g_rightBorder);
        g_allBlackholes = realloc(g_allBlackholes, g_amountBlackHoles * sizeof(*g_allBlackholes));
        //Falls BH dazu kommet, wird ein neues generiert
        if (v == 1) {
            g_allBlackholes[g_amountBlackHoles - 1].innerRadius = CALC_INNER_BH_RADIUS(width);
            g_allBlackholes[g_amountBlackHoles - 1].outerRadius = CALC_OUTER_BH_RADIUS(width);

            g_allBlackholes[g_amountBlackHoles - 1].center[CX] = RANDOM_IN_RANGE(2.0f);
            g_allBlackholes[g_amountBlackHoles - 1].center[CY] = 0.0f;
            g_allBlackholes[g_amountBlackHoles - 1].center[CZ] = RANDOM_IN_RANGE(2.0f);
            calcInterpolBlackHolePos(GL_TRUE);
        }
    }
}

/**
 * Updated die Kontrollvertizes. Fuegt oder entfernt eine Reihe und Spalte hinzu
 * @param state +1/-1
 * @return Zeiger auf neuen Kontrollvertizes
 */
GLfloat *updateLogic(expandShrinkVertices state) {
    GLint i = 0;
    GLint rowCount = -1;
    GLint oldAmountVerticesSide = g_amountVerticesSide;
    GLint newAmountVerticesSide = oldAmountVerticesSide + state;
    g_amountVerticesSide = newAmountVerticesSide;

    //alte Hoehen speichern um diese beim Vergroeßern oder Verkleinern nicht zu verlieren
    GLfloat *oldHeights = calloc(SQUARE(oldAmountVerticesSide), sizeof(GLfloat));
    if (oldHeights == NULL) {
        exit(1);
    }


    for (i = 0; i < SQUARE(oldAmountVerticesSide); i++) {
        oldHeights[i] = g_controlVertices[i][CY];
    }

    //Speicher des Hoehenarrays realloziieren
    GLfloat *heights = calloc(SQUARE(newAmountVerticesSide), sizeof(GLfloat));
    if (heights == NULL) {
        exit(1);
    }

    //Fuellen des neuen Hoehen- und Geschwindigkeitsrrays
    for (i = 0; i < SQUARE(newAmountVerticesSide); i++) {
        if ((i % newAmountVerticesSide) == 0) {
            rowCount++;
        }

        if (state == expand) {
            //neue Punkte haben die Hoehe und Geschwindigkeit 0
            if (((i % newAmountVerticesSide) == (newAmountVerticesSide - 1)) ||
                ((i / newAmountVerticesSide) == (newAmountVerticesSide - 1))) {
                heights[i] = 0.0f;
            }
                //alte Punkte behalten ihre Werte
            else {
                heights[i] = oldHeights[i - rowCount];
            }
        }
            //Beim Verkleinern werden die Randpunkte einfach abgeschnitten
        else {
            heights[i] = oldHeights[i + rowCount];
        }
    }
    //Freigabe der Zwischenspeicher
    free(oldHeights);
    return heights;
}

/**
 * Beschleunigt den ersten Ball der noch im Spiel ist, in eine zufällige x-z-Richtung
 */
void kickFirstBall() {
    GLint i = 0;
    while ((i < g_amountBalls) && !g_allBalls[i].active) {
        i++;
    }
    firstInPlayBall = i;
    g_allBalls[firstInPlayBall].vel[CX] += RANDOM_KICK;
    g_allBalls[firstInPlayBall].vel[CZ] += RANDOM_KICK;
}

/**
 * Aktualisiert die Farben aller Kugeln.
 * Die Farben sind vom Hoehenwert der jeweiligen Kugel abghaengig
 */
void updateColorSelectedControlVert() {
    CGColor3f color = {1.0f, 1.0f, 1.0f};
    for (GLint i = 0; i < SQUARE(getAmountVerticesSide()); i++) {
        //Farbe fuer niedrige Hoehen

        if (i == selectedVert) {
            color[0] = COLOR_WHITE[2];
            color[1] = COLOR_WHITE[2];
            color[2] = COLOR_WHITE[2];
        } else {
            color[0] = LOW_DARK_BLUE[0];
            color[1] = LOW_DARK_BLUE[1];
            color[2] = LOW_DARK_BLUE[2];
        }

        g_controlVertices[i][CR] = color[0];
        g_controlVertices[i][CG] = color[1];
        g_controlVertices[i][CB] = color[2];
    }
    updateScene();
}


void updateVertexArray(expandShrinkVertices state) {
    GLfloat currX = -1.0f;
    GLfloat currZ;

    //neue Hoehen aus der Logik holen
    GLfloat *newHeights = updateLogic(state);
    if (newHeights != NULL) {

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
            free(newHeights);
        }
    } else {
        exit(1);
    }
    calcBorders(&g_upperBorder, &g_lowerBorder, &g_leftBorder, &g_rightBorder);
}

void pickedVertex(GLuint index, int direction) {
    g_controlVertices[index][CY] += direction * PICK_HEIGHT;
}


void updateSelectedHeight(int direction) {
    pickedVertex(selectedVert, direction);
    updateSplineVertexArray(SQUARE(g_amountSplineVertices));
}

void updateSplineVertices(GLint v) {
    GLint splineVerts = getAmountSplineVertices();
    if (splineVerts + v >= MIN_SPLINE_AMOUNT
        && splineVerts + v <= MAX_SPLINE_AMOUNT) {
        updateAmountSplineVertices(v);
        splineVerts = getAmountSplineVertices();

        //Neuberechnung der interpolierten Punkte
        g_splineSurface = realloc(g_splineSurface, sizeof(splineVertex) * SQUARE(splineVerts));
        updateSplineVertexArray(SQUARE(splineVerts));
        updateIndexArray(SQUARE(splineVerts - 1) * 2 * 3);
        updateScene();
    }
}

/**
 * Liefert den Status der Lichtberechnung.
 * @return Status der Lichtberechnung (an/aus).
 */
int getLightingState(void) {
    return g_lightingState;
}

/**
 * Setzt den Status der Lichtberechnung.
 * @param status Status der Lichtberechnung (an/aus).
 */
void setLightingState(int status) {
    g_lightingState = status;
}

/**
 * Liefert den Status der ersten Lichtquelle.
 * @return Status der ersten Lichtquelle (an/aus).
 */
int getLight0State(void) {
    return g_light0State;
}

/**
 * Setzt den Status der ersten Lichtquelle.
 * @param status Status der ersten Lichtquelle (an/aus).
 */
void setLight0State(int status) {
    g_light0State = status;
}

/**
 * Berechnet aktuellen Rotationswinkel der zweiten Lichtquelle.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void calcLight1Rotation(double interval) {
    if ((g_light1State == rotating) && g_lightingState) {
        g_light1RotationAngle += 360.0f * LIGHT1_ROTATIONS_PS * (float) interval;
    }
}

/**
 * Liefert den aktuellen Rotationswinkel der zweiten Lichtquelle.
 * @return aktueller Rotationswinkel der zweiten Lichtquelle.
 */
float getLight1Rotation(void) {
    return g_light1RotationAngle;
}

void updateSelectedVert(int i) {
    selectedVert = selectedVert + i;
    if (selectedVert >= SQUARE(g_amountVerticesSide)) {
        selectedVert = 0;
    } else if (selectedVert < 0) {
        selectedVert = SQUARE(g_amountVerticesSide) - 1;
    }
}

GLint getAmountVerticesSide() {
    return g_amountVerticesSide;
}

GLint getAmountSplineVertices() {
    return g_amountSplineVertices;
}

void updateAmountVertices(GLint v) {
    g_amountVerticesSide += v;
}

void updateAmountSplineVertices(GLint v) {
    g_amountSplineVertices += v;
}

ball *getAllBalls() {
    return g_allBalls;
}

GLint getAmountBalls() {
    return g_amountBalls;
}

controlVertex *getControlVertices() {
    return g_controlVertices;
}

splineVertex *getSplineVertices() {
    return g_splineSurface;
}

void toggleStartBalls() {
    startBalls = !startBalls;
}

CGPoint3f *getGoalSphere(void) {
    return &g_goalSphere;
}

blackHole *getAllBlackHoles() {
    return g_allBlackholes;
}

GLint getAmountBlackHoles() {
    return g_amountBlackHoles;
}

void toggleWinning(){
    g_enableWinning = !g_enableWinning;
}

GLfloat getBallRadius(){
    return ballRadius;
}
