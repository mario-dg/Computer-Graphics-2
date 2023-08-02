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

#endif

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "debugGL.h"
#include "stringOutput.h"
#include "logic.h"
#include "texture.h"
#include "sceneObjects.h"
#include "euler_integration.h"
#include <math.h>

static const CGColor3f COLOR_BROWN = {0.59f, 0.29f, 0.0f};
/**Kameraposition im KugelKoordinatensystem */
static GLfloat cameraRadiussph = 0.0f;
static GLfloat cameraThetaSph = 0.0f;
static GLfloat cameraPhiSph = 0.0f;

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
GLboolean drawTriangles = GL_FALSE;
GLboolean drawShaddows = GL_FALSE;
GLboolean drawNormals = GL_FALSE;

static particle *g_particles;
static CGVector3f *g_goals;


/**
* Gibt den Hilfetext aus.
*/
static void drawHelp() {
    int size = 17;

    GLfloat color[3] = {LOW_DARK_BLUE[0], LOW_DARK_BLUE[1], LOW_DARK_BLUE[2]};

    char *help[] = {"Hilfe",
                    "F1 - Wireframe an/aus",
                    "F2 - Normalen an/aus",
                    "F3 - Beleuchtungsberechnung an/aus",
                    "F12 - Vollbildmodus an/aus",
                    "ESC/q/Q - Ende",
                    "h/H - Hilfe an/aus",
                    "v/V - Texturierung an/aus",
                    "t/T - Wechseln der Texturen",
                    "p/P - Pausieren",
                    "r/R - Normalen zeichnen",
                    "s/S - Schatten An-/Ausstellen",
                    "i,I/j,J/k,K/l,L - Rotation der Kamera",
                    "m/M - Ziele in Bewegung setzen",
                    "n/N - Ziel-Partikel wechseln",
                    "d/D - Darstellungsmodus fuer Partikel wechseln (Linie/Dreieck)",
                    "z/Z - Zielobjekt wechseln (Kugeln/Partikel/Zentrum)"};

    drawString(0.3f, 0.04f, color, help[0]);

    for (GLint i = 1; i < size; ++i) {
        drawString(0.1f, 0.025f + i * 0.04f, color, help[i]);
    }
}

void updateScene(void) {
    g_goals = getAllGoals();
    g_particles = getAllParticles();
}

/**
 * Zeichnet eine Linie mit der uebergebenen Farbe,
 * mit der Laenge 1 entlang der z-Achse
 *
 * @param[in] color, Farbe des Quadrats
 */
static void drawLine(GLint i, const CGColor3f color) {
    glBegin(GL_LINES);
    {
        glColor3f(color[0], color[1], color[2]);
        glVertex3f(g_particles[i].up[0], g_particles[i].up[1], g_particles[i].up[2]);
        glVertex3f(0.0f, 0.0f, 0.0f);
    }
    glEnd();
}

/**
 * Zeichnet den Koerper des Partikels als Linie
 * @param i zu zeichnendes Partikel
 */
static void drawLineBody(GLint i) {
    glBegin(GL_LINES);
    {
        if (i == getSelectedParticle() && (getFollowMode() == followSingle)) {
            //Farbe des ZielPartikels(falls followSingle ausgewaehlt ist)
            glColor3f(HIGH_RED[0], HIGH_RED[1], HIGH_RED[2]);
        } else {
            glColor3f(COLOR_YELLOW[0], COLOR_YELLOW[1], COLOR_YELLOW[2]);
        }

        glVertex3f(g_particles[i].vel[0] * 0.5, g_particles[i].vel[1] * 0.5, g_particles[i].vel[2] * 0.5);

        if (i == getSelectedParticle() && (getFollowMode() == followSingle)) {
            //Farbe des ZielPartikels(falls followSingle ausgewaehlt ist)
            glColor3f(LOW_DARK_BLUE[0], LOW_DARK_BLUE[1], LOW_DARK_BLUE[2]);
        } else {
            glColor3f(MEDIUM_GREEN[0], MEDIUM_GREEN[1], MEDIUM_GREEN[2]);
        }

        glVertex3f(-g_particles[i].vel[0] * 0.5, -g_particles[i].vel[1] * 0.5, -g_particles[i].vel[2] * 0.5);
    }
    glEnd();

}

/**
 * Zeichnet die Schatten fuer ein lineares Partikel
 * @param i Partikelnummer, fuer die SChatten gezeichnet werden soll
 */
void drawLineShaddow(GLint i) {
    //Schattenfarbe
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    {
        glVertex3f(g_particles[i].vel[0] * 0.5, 0.0f, g_particles[i].vel[2] * 0.5);

        glVertex3f(-g_particles[i].vel[0] * 0.5, 0.0f, -g_particles[i].vel[2] * 0.5);
    }
    glEnd();
}

/**
 * Zeichnet den Koerper des Partikels als Dreieck
 * @param i zu zeichnendes Partikel
 */
static void drawTriangleBody(GLint i) {
    //Seiten-Vektor fuer die Ausrichtung des Partikels berrechnen
    CGVector3f sideVec;
    crossProduct(g_particles[i].vel, g_particles[i].up, sideVec);

    //Partikel zeichnen
    glBegin(GL_TRIANGLES);
    {
        //linker Fluegel
        if (i == getSelectedParticle() && (getFollowMode() == followSingle)) {
            //Farbe des ZielPartikels(falls followSingle ausgewaehlt ist)
            glColor3f(HIGH_RED[0], HIGH_RED[1], HIGH_RED[2]);
        } else {

            glColor3f(COLOR_YELLOW[0], COLOR_YELLOW[1], COLOR_YELLOW[2]);
        }

        glVertex3f(g_particles[i].vel[0] * 0.5, g_particles[i].vel[1] * 0.5, g_particles[i].vel[2] * 0.5);
        glVertex3f(-g_particles[i].vel[0] * 0.5, -g_particles[i].vel[1] * 0.5, -g_particles[i].vel[2] * 0.5);
        glVertex3f(sideVec[0] - g_particles[i].vel[0] * 0.5, sideVec[1] - g_particles[i].vel[1] * 0.5,
                   sideVec[2] - g_particles[i].vel[2] * 0.5);

        glVertex3f(g_particles[i].vel[0] * 0.5, g_particles[i].vel[1] * 0.5, g_particles[i].vel[2] * 0.5);
        glVertex3f(sideVec[0] - g_particles[i].vel[0] * 0.5, sideVec[1] - g_particles[i].vel[1] * 0.5,
                   sideVec[2] - g_particles[i].vel[2] * 0.5);
        glVertex3f(-g_particles[i].vel[0] * 0.5, -g_particles[i].vel[1] * 0.5, -g_particles[i].vel[2] * 0.5);

        //rechter Fluegel
        if (i == getSelectedParticle() && (getFollowMode() == followSingle)) {
            //Farbe des ZielPartikels(falls followSingle ausgewaehlt ist)
            glColor3f(LOW_DARK_BLUE[0], LOW_DARK_BLUE[1], LOW_DARK_BLUE[2]);
        } else {
            glColor3f(MEDIUM_GREEN[0], MEDIUM_GREEN[1], MEDIUM_GREEN[2]);
        }
        glVertex3f(g_particles[i].vel[0] * 0.5, g_particles[i].vel[1] * 0.5, g_particles[i].vel[2] * 0.5);
        glVertex3f(-g_particles[i].vel[0] * 0.5, -g_particles[i].vel[1] * 0.5, -g_particles[i].vel[2] * 0.5);
        glVertex3f(-sideVec[0] - g_particles[i].vel[0] * 0.5, -sideVec[1] - g_particles[i].vel[1] * 0.5,
                   -sideVec[2] - g_particles[i].vel[2] * 0.5);

        glVertex3f(g_particles[i].vel[0] * 0.5, g_particles[i].vel[1] * 0.5, g_particles[i].vel[2] * 0.5);
        glVertex3f(-sideVec[0] - g_particles[i].vel[0] * 0.5, -sideVec[1] - g_particles[i].vel[1] * 0.5,
                   -sideVec[2] - g_particles[i].vel[2] * 0.5);
        glVertex3f(-g_particles[i].vel[0] * 0.5, -g_particles[i].vel[1] * 0.5, -g_particles[i].vel[2] * 0.5);

    }
    glEnd();
}

/**
 * Zeichnet einen Dreiecksschatten fuer ein Partikel
 * @param i Partikelnummer, fuer die SChatten gezeichnet werden soll
 */
void drawTriangleShaddow(GLint i) {
    //Seiten-Vektor fuer die Ausrichtung des Partikelschattens berrechnen
    CGVector3f sideVec;
    crossProduct(g_particles[i].vel, g_particles[i].up, sideVec);

    //Schatten zeichnen
    glBegin(GL_TRIANGLES);
    {
        //Schattenfarbe
        glColor3f(0.0f, 0.0f, 0.0f);
        //erster Fluegel
        glVertex3f(g_particles[i].vel[0] * 0.5, 0.0f, g_particles[i].vel[2] * 0.5);
        glVertex3f(-g_particles[i].vel[0] * 0.5, 0.0f, -g_particles[i].vel[2] * 0.5);
        glVertex3f(sideVec[0] - g_particles[i].vel[0] * 0.5, 0.0f,
                   sideVec[2] - g_particles[i].vel[2] * 0.5);

        glVertex3f(g_particles[i].vel[0] * 0.5, 0.0f, g_particles[i].vel[2] * 0.5);
        glVertex3f(sideVec[0] - g_particles[i].vel[0] * 0.5, 0.0f,
                   sideVec[2] - g_particles[i].vel[2] * 0.5);
        glVertex3f(-g_particles[i].vel[0] * 0.5, 0.0f, -g_particles[i].vel[2] * 0.5);

        //zweiter fluegel
        glVertex3f(g_particles[i].vel[0] * 0.5, 0.0f, g_particles[i].vel[2] * 0.5);
        glVertex3f(-g_particles[i].vel[0] * 0.5, 0.0f, -g_particles[i].vel[2] * 0.5);
        glVertex3f(-sideVec[0] - g_particles[i].vel[0] * 0.5, 0.0f,
                   -sideVec[2] - g_particles[i].vel[2] * 0.5);

        glVertex3f(g_particles[i].vel[0] * 0.5, 0.0f, g_particles[i].vel[2] * 0.5);
        glVertex3f(-sideVec[0] - g_particles[i].vel[0] * 0.5, 0.0f,
                   -sideVec[2] - g_particles[i].vel[2] * 0.5);
        glVertex3f(-g_particles[i].vel[0] * 0.5, 0.0f, -g_particles[i].vel[2] * 0.5);
    }
    glEnd();
}

/**
 * Zeichnet eine Normale von einem Punkt im Partikel-Array
 * @param i Index des Punktes von dem die Normale gezeichnet wird
 */
static void drawNormal(GLint i) {
    glPushMatrix();
    {
        glTranslatef(g_particles[i].center[0], g_particles[i].center[1], g_particles[i].center[2]);
        glScalef(1.0f / 10, 1.0f / 10, 1.0f / 10);
        drawLine(i, LOW_DARK_BLUE);
    }
    glPopMatrix();
}


/**
 * Zeichnet alle Partikel
 */
void drawAllParticles(void) {

    glPushMatrix();
    {
        glDisable(GL_TEXTURE_2D);
        for (int i = 0; i < getParticleCount(); i++) {
            glPushMatrix();
            {
                //Partikel an richtige Position verschieben
                glTranslatef(g_particles[i].center[X], g_particles[i].center[Y], g_particles[i].center[Z]);
                //Partikel skallieren
                glScalef(1.0f / 7, 1.0f / 7, 1.0f / 7);

                //Zeichenmodus waehlen
                if (drawTriangles) {
                    drawTriangleBody(i);
                } else {
                    drawLineBody(i);
                }
            }
            glPopMatrix();
        }
        if (drawShaddows) {
            for (int i = 0; i < getParticleCount(); ++i) {
                glPushMatrix();
                {
                    //Partikel an richtige Position verschieben
                    glTranslatef(g_particles[i].center[X], -AREA_WIDTH / 2 + 0.001f, g_particles[i].center[Z]);
                    //Partikel skallieren
                    glScalef(1.0f / 7, 1.0f / 7, 1.0f / 7);

                    //Zeichenmodus waehlen
                    if (drawTriangles) {
                        drawTriangleShaddow(i);
                    } else {
                        drawLineShaddow(i);
                    }
                }
                glPopMatrix();
            }
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet alle Ziele
 */
void drawAllGoals(void) {
    glPushMatrix();
    {
        glDisable(GL_TEXTURE_2D);
        for (int i = 0; i < getGoalCount(); i++) {
            glPushMatrix();
            {
                //Ziele an richtige Position verschieben
                glTranslatef(g_goals[i][X], g_goals[i][Y], g_goals[i][Z]);
                //Ziele skallieren
                glScalef(1.0f / 15, 1.0f / 15, 1.0f / 15);

                //Farbe des Zieles festlegen und zeichnen
                const CGColor3f color = {0.0f, 0.0f, 0.0f};
                drawSphere(color);
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet die vier Waende und einen Boden
 */
void drawBox(void) {
    if (getTexturingStatus()) {
        /* Texturierung aktivieren */
        glEnable(GL_TEXTURE_2D);
    }
    bindTexture(usedTexture);
    //BACK_WALL
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.0f, -AREA_WIDTH / 2.0f);
        glRotatef(90.0f, 1, 0, 0);
        glScalef(AREA_WIDTH, AREA_WIDTH, AREA_WIDTH);
        drawPlane(COLOR_WHITE);
		
		if (drawNormals)
		{
            glPushMatrix();
            {
                glScalef(1.0f / AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f);
                drawNormalPlane(COLOR_BROWN);
            }
            glPopMatrix();
		}
    }
    glPopMatrix();

    //FRONT_WALL
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.0f, AREA_WIDTH / 2.0f);
        glRotatef(-90.0f, 1, 0, 0);
        glScalef(AREA_WIDTH, AREA_WIDTH, AREA_WIDTH);
        drawPlane(COLOR_WHITE);

        if (drawNormals)
        {
            glPushMatrix();
            {
                glScalef(1.0f / AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f);
                drawNormalPlane(COLOR_BROWN);
            }
            glPopMatrix();
        }
    }
    glPopMatrix();

    //LEFT_WALL
    glPushMatrix();
    {
        glTranslatef(-AREA_WIDTH / 2.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0, 0, 1);
        glScalef(AREA_WIDTH, AREA_WIDTH, AREA_WIDTH);
        drawPlane(COLOR_WHITE);

        if (drawNormals)
        {
            glPushMatrix();
            {
                glScalef(1.0f / AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f);
                drawNormalPlane(COLOR_BROWN);
            }
            glPopMatrix();
        }
    }
    glPopMatrix();

    //RIGHT_WALL
    glPushMatrix();
    {
        glTranslatef(AREA_WIDTH / 2.0f, 0.0f, 0.0f);
        glRotatef(90.0f, 0, 0, 1);
        glScalef(AREA_WIDTH, AREA_WIDTH, AREA_WIDTH);
        drawPlane(COLOR_WHITE);

        if (drawNormals)
        {
            glPushMatrix();
            {
                glScalef(1.0f / AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f);
                drawNormalPlane(COLOR_BROWN);
            }
            glPopMatrix();
        }
    }
    glPopMatrix();

    //TOP
    glPushMatrix();
    {
        glTranslatef(0.0f, AREA_WIDTH / 2.0f, 0.0f);
        glRotatef(180.0f, 1, 0, 0);
        glScalef(AREA_WIDTH, AREA_WIDTH, AREA_WIDTH);
        drawPlane(COLOR_WHITE);

        if (drawNormals)
        {
            glPushMatrix();
            {
                glScalef(1.0f / AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f);
                drawNormalPlane(COLOR_BROWN);
            }
            glPopMatrix();
        }
    }
    glPopMatrix();

    bindTexture(texBottom1);
    //BOTTOM
    glPushMatrix();
    {
        glTranslatef(0.0f, -AREA_WIDTH / 2.0f, 0.0f);
        glScalef(AREA_WIDTH, AREA_WIDTH, AREA_WIDTH);
        drawPlane(COLOR_WHITE);

        if (drawNormals)
        {
            glPushMatrix();
            {
                glScalef(1.0f / AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f, 1.0f/ AREA_WIDTH / 10.0f);
                drawNormalPlane(COLOR_BROWN);
            }
            glPopMatrix();
        }
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

/**
 * Zeichen-Funktion, stellt die Szene dar
 */
void drawScene(void) {
    if (showHelp) {
        drawHelp();
    } else {

        drawBox();
        /* Punktlichtquelle, die die Szene von oben beleuchtet */
        CGPoint4f lightPos0 = {0.0f, 1.5f, 0.0f, 0.0f};

        /* Spotlichtquelle, entweder rotierend */
        CGVector3f lightDirection0 = {0, -3.0f, 0};

        if (getLightingState()) {
            /* Lichtberechnung aktivieren */
            glEnable(GL_LIGHTING);

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

        /* Partikel zeichnen */
        drawAllParticles();
        /* Ziele zeichnen */
        drawAllGoals();

        //Falls gewuenscht Normalen zeichnen
        glPushMatrix();
        {
            if (getShowNormal()) {
                for (int i = 0; i < getParticleCount(); ++i) {
                    drawNormal(i);
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
    glClearColor(0.7f, 0.7f, 0.7f, 0.5f);
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

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    initLight();
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

/**
 * (De-)aktiviert die Anzeige der Schatten.
 */
void toggleShaddows(void) {
    drawShaddows = !drawShaddows;
}

/**
 * (De-)aktiviert die Anzeige der Normalen.
 */
void toggleNormal(void) {
    drawNormals = !drawNormals;
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

void toggleParticleDrawMode(void) {
    drawTriangles = !drawTriangles;
}

void toggleShowHelp(void) {
    showHelp = !showHelp;
}

void nextTexture() {
    usedTexture++;
    usedTexture = usedTexture % (TEX_COUNT - 1);
}
