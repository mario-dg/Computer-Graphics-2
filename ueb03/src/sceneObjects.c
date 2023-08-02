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

#include <stdio.h>
#include <math.h>
#include "types.h"
#include "scene.h"
#include "logic.h"

#define SLICES (8)

GLboolean showNormal = GL_FALSE;

/**
 * Zeichnen einer Einheitskugel
 * @param color Farbe der Kugel
 */
void drawSphere(const CGColor3f color) {
    /* Material der Kugel*/
    float matShininess[] = {128.0f};

    /* Setzen der Material-Parameter fuer die Beleuchtung */
    CGColor3f temp = {0.75f, 0.75f, 0.75f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, temp);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, temp);
    glMaterialfv(GL_FRONT, GL_SPECULAR, temp);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    const GLfloat radius = 1.0f;

    glPushMatrix();
    {
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        /* Quadric erzuegen */
        GLUquadricObj *qobj = gluNewQuadric();

        if (qobj != 0) {
            glPushMatrix();
            {
                glColor3f(color[0], color[1], color[2]);
                /* Gefuellt darstellen */
                gluQuadricDrawStyle(qobj, GLU_FILL);
                /* Scheibe zeichnen */
                gluSphere(qobj, radius, SLICES, SLICES);
                gluQuadricNormals(qobj, GLU_SMOOTH);
                /* Loeschen des Quadrics nicht vergessen */
                gluDeleteQuadric(qobj);
            }
            glPopMatrix();
        } else {
            fprintf(stderr, "Erzeugen der Kugeln fehlgeschlagen\n");
        }
    }
    glPopMatrix();
}

/**
 * Zeichnen einer transparenten Einheitskugel
 * @param color Farbe der transparenten Kugel
 */
void drawTransparentSphere(const CGColor4f color) {
    /* Material der Kugel*/
    float matShininess[] = {128.0f};

    /* Setzen der Material-Parameter fuer die Beleuchtung */
    CGColor3f temp = {0.75f, 0.75f, 0.75f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, temp);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, temp);
    glMaterialfv(GL_FRONT, GL_SPECULAR, temp);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    const GLfloat radius = 1.0f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    {
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        /* Quadric erzuegen */
        GLUquadricObj *qobj = gluNewQuadric();

        if (qobj != 0) {
            glPushMatrix();
            {
                glColor4f(color[0], color[1], color[2], color[3]);
                /* Gefuellt darstellen */
                gluQuadricDrawStyle(qobj, GLU_FILL);
                /* Scheibe zeichnen */
                gluSphere(qobj, radius, SLICES * 3, SLICES * 3);
                gluQuadricNormals(qobj, GLU_SMOOTH);
                /* Loeschen des Quadrics nicht vergessen */
                gluDeleteQuadric(qobj);
            }
            glPopMatrix();
        } else {
            fprintf(stderr, "Erzeugen der Kugeln fehlgeschlagen\n");
        }
    }
    glPopMatrix();
    glDisable(GL_BLEND);
}

/**
 * Zeichnen aller Baelle, welche aktiv sind
 */
void drawBalls() {
    ball *allBalls = getAllBalls();
    const CGColor3f purple = {1.0f, 0.0f, 1.0f};
    GLfloat radius = getBallRadius();
    //Ueber alle Baelle laufen
    for (int i = 0; i < getAmountBalls(); ++i) {
        //Nur zeichnen, wenn der Ball aktiv ist
        if (allBalls[i].active) {
            glPushMatrix();
            {
                //Kugel zeichnen, skalieren und translatieren
                glTranslatef(allBalls[i].center[CX], allBalls[i].center[CY] + radius, allBalls[i].center[CZ]);
                glScalef(radius, radius, radius);
                drawSphere(purple);
            }
            glPopMatrix();
        }
    }
}

/**
 * Zeichnen aller Schwarzen Loecher
 */
void drawBlackHoles() {
    blackHole *allBlackHoles = getAllBlackHoles();
    GLfloat radius = allBlackHoles[0].innerRadius;
    GLfloat outerRadius = allBlackHoles[0].outerRadius;
    const CGColor3f black = {0.0f, 0.0f, 0.0f};
    //Ueber alle Schwarzen Loecher laufen
    for (int i = 0; i < getAmountBlackHoles(); ++i) {
        //Mittelpunkt des SChwarzen Loches zeichnen
        glPushMatrix();
        {
            glTranslatef(allBlackHoles[i].center[CX], allBlackHoles[i].center[CY], allBlackHoles[i].center[CZ]);
            glScalef(radius, radius, radius);
            drawSphere(black);
        }
        glPopMatrix();

        //Transparenten Teil des Schwarzen Loches zeichnen
        glPushMatrix();
        {
            glTranslatef(allBlackHoles[i].center[CX], allBlackHoles[i].center[CY], allBlackHoles[i].center[CZ]);
            glScalef(outerRadius, outerRadius, outerRadius);
            drawTransparentSphere(COLOR_TRANS_BLACK);
        }
        glPopMatrix();
    }
}

GLboolean getShowNormal(void) {
    return showNormal;
}

void setShowNormal(GLboolean normal) {
    showNormal = normal;
}
