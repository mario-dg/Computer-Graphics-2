#ifndef __TYPES_H__
#define __TYPES_H__
/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Mario da Graca, Christopher Ploog
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/** Makro, um das Quadrat einer Zahl zu bestimmen*/
#define SQUARE(x) ((x) * (x))

/* ---- Konstanten ---- */
// Default Fenster Groesse
#define DEFAULT_WINDOW_WIDTH 600
#define DEFAULT_WINDOW_HEIGHT 600
#define EPSILON (0.00001f)
/** Anzahl der Texturen. */
#define TEX_COUNT (3)

/* ---- Typedeklarationen ---- */


//Konstanten und Typen zur besseren Lesbarkeit
#define CX (0)
#define CY (1)
#define CZ (2)

#define CR (3)
#define CG (4)
#define CB (5)

#define CNX (6)
#define CNY (7)
#define CNZ (8)

#define CTX (9)
#define CTY (10)

//Typdefinition fuer einen Vertex
typedef GLfloat splineVertex[11];

typedef GLfloat controlVertex[6];

//Anzahl der Punkte pro Seite, aus denen das Mesh initial aufgebaut ist
#define START_AMOUNT_VERTICES (4)


/** RGB-Farbwerte */
typedef GLfloat CGColor3f[3];

typedef GLfloat CGColor4f[4];

/** Vektor im 3D-Raum*/
typedef GLfloat CGVector3f[3];

typedef GLfloat CGVector2f[2];

typedef GLfloat CGMatrix44f[4][4];
typedef GLfloat CGMatrix14f[1][4];
typedef GLfloat CGMatrix41f[4][1];

/** Koordinate im 2D-Raum */
typedef GLint CGCoord2i[2];
/** Koordinate im 3D-Raum */
typedef GLfloat CGPoint3f[3];
/** Koordinate im 4D-Raum */
typedef GLfloat CGPoint4f[4];
/** enum fuer den Status des rotierenden Lichts*/
typedef enum e_light1State
{
  rotating,
  off
} light1State;

typedef enum e_cameraMove
{
    forward,
    back,
    left,
    right
} cameraMove;

typedef enum e_calculationMode
{
    interpolation,
    derivative_s,
    derivative_t
} calculationMode;

/** enum fuer die Anpassung des Mesh*/
typedef enum e_expandShrinkVertices
{
  expand = 1,
  shrink = -1
} expandShrinkVertices;

#endif
