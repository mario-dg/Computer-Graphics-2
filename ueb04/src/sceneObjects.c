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
 * Zeichnet eine Linie mit der uebergebenen Farbe,
 * mit der Laenge 1 entlang der z-Achse
 *
 * @param[in] color, Farbe des Quadrats
 */
static void drawLine(const CGColor3f color)
{
  /* Material des Wuerfels */
  float matShininess[] = {5.0f};

  /* Setzen der Material-Parameter */
  glMaterialfv(GL_FRONT, GL_AMBIENT, color);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT, GL_SPECULAR, color);
  glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

  glBegin(GL_LINES);
  {
    glColor3f(color[0], color[1], color[2]);
    glVertex3f(0.0f, 0.0f, -0.5f);
    glVertex3f(0.0f, 0.0f, 0.5f);
  }
  glEnd();
}

void drawNormalPlane(const CGColor3f color)
{
  glPushMatrix();
  {
    glLineWidth(2.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    glScalef(0.5f, 1.0f, 1.0f);
    drawLine(color);
  }
  glPopMatrix();
}

void drawPlane(const CGColor3f color) {
    /* Material des Wuerfels */
    float matShininess[] = {25.0f};

    /* Setzen der Material-Parameter */
    glMaterialfv(GL_FRONT, GL_AMBIENT, color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, color);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    glBegin(GL_QUADS);
    {
        glNormal3f(0.0f, 1.0f, 0.0f);
        glColor3f(color[0], color[1], color[2]);
        glVertex3f(-0.5f, 0.0f, -0.5f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-0.5f, 0.0f, 0.5f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.5f, 0.0f, 0.5f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.5f, 0.0f, -0.5f);
        glTexCoord2f(1.0f, 0.0f);
    }
    glEnd();
}

GLboolean getShowNormal(void) {
    return showNormal;
}

void setShowNormal(GLboolean normal) {
    showNormal = normal;
}
