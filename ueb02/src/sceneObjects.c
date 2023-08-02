/**
 * @file
 * Extra-Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet fuer ausgelagerte Zeichenfunktionen
 *
 * @author Mario da Graca, Christopher Ploog
 */

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

#define SLICES (8)

GLboolean showNormal = GL_FALSE;

void drawSphere(const CGColor3f color)
{
  /* Material der Kugel*/
  float matShininess[] = {128.0f};

  /* Setzen der Material-Parameter fuer die Beleuchtung */
  CGColor3f temp = {0.75f, 0.75f, 0.75f};
  glMaterialfv(GL_FRONT, GL_AMBIENT, temp);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, temp);
  glMaterialfv(GL_FRONT, GL_SPECULAR, temp);
  glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

  const GLfloat radius = 0.5f;

  glPushMatrix();
  {
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    /* Quadric erzuegen */
    GLUquadricObj *qobj = gluNewQuadric();

    if (qobj != 0)
    {
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
    }
    else
    {
      fprintf(stderr, "Erzeugen der Kugeln fehlgeschlagen\n");
    }
  }
  glPopMatrix();
}

GLboolean getShowNormal(void)
{
  return showNormal;
}

void setShowNormal(GLboolean normal)
{
  showNormal = normal;
}
