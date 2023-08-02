#ifndef __TYPES_H__
#define __TYPES_H__
/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
 *
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Christopher Ploog, Mario da Graca
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
/** Default Fenster Groesse */
#define DEFAULT_WINDOW_WIDTH 600
#define DEFAULT_WINDOW_HEIGHT 600

/** Epsilon fuer Float vergleiche */
#define EPSILON (0.00001f)

/** Anzahl der Texturen. */
#define TEX_COUNT (3)

/** Startwerte */
#define DEFAULT_AMOUNT_BALLS (10)
#define DEFAULT_AMOUNT_BH (5)
#define DEFAULT_AMOUNT_VERTICES (6)
#define DEFAULT_SPLINE_AMOUNT (15)

/** Minimale und Maximale Werte von Aspekten, die sich im
 *  Spielverlauf veraendern lassen
 */
#define MIN_SPLINE_AMOUNT (2)
#define MAX_SPLINE_AMOUNT (60)
#define MIN_AMOUNT_BH (1)
#define MAX_AMOUNT_BH (15)

/** Grenzwerte fuer Faerbung der Flaeche je nach Hoehe */
#define LOWER_BORDER (0.15f)
#define MIDDLE_BORDER (0.3f)
#define UPPER_BORDER (0.45f)

/** Konstanten fuer Baelle, Ziel */
#define DEFAULT_BALL_RADIUS (0.04f)
#define GOAL_RADIUS (0.1f)

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

/* ---- Typedeklarationen ---- */

/** Typdefinition fuer einen Splinevertex */
typedef GLfloat splineVertex[11];

/** Typdefinition fuer einen Kontrollvertex */
typedef GLfloat controlVertex[6];

/** RGB-Farbwerte */
typedef GLfloat CGColor3f[3];

/** RGBA-Farbwert */
typedef GLfloat CGColor4f[4];

/** Vektor im 3D-Raum*/
typedef GLfloat CGVector3f[3];


/** Datentypem um Matrizen darzustellen */
typedef GLfloat CGMatrix44f[4][4];
typedef GLfloat CGMatrix14f[1][4];
typedef GLfloat CGMatrix41f[4][1];

/** Koordinate im 2D-Raum */
typedef GLint CGCoord2i[2];
/** Koordinate im 3D-Raum */
typedef GLfloat CGPoint3f[3];
/** Koordinate im 4D-Raum */
typedef GLfloat CGPoint4f[4];

/** Enum fuer den Status des rotierenden Lichts*/
typedef enum e_light1State
{
  rotating,
  off
} light1State;

/** Enum um die Kamerabewegung zu identifizieren */
typedef enum e_cameraMove
{
    forward,
    back,
    left,
    right
} cameraMove;

/** Enum um den Berechnungsmodus der Splineflaeche umzustellen */
typedef enum e_calculationMode
{
    interpolation,
    derivative_s,
    derivative_t
} calculationMode;

/** Enum fuer die Anpassung des Mesh*/
typedef enum e_expandShrinkVertices
{
  expand = 1,
  shrink = -1
} expandShrinkVertices;


/** Struct um einen Ball darzustellen*/
typedef struct ball {
    GLint id;
    GLboolean active;
    CGPoint3f center;
    CGVector3f acc;
    CGVector3f vel;
} ball;

/** Struct um ein schwarzes Loch darzustellen*/
typedef struct blackHole{
    CGPoint3f center;
    GLfloat innerRadius;
    GLfloat outerRadius;
} blackHole;

/** Enum um alle 4 Grenzen darzustellen*/
typedef enum e_Borders{
    leftBorder,
    rightBorder,
    upperBorder,
    lowerBorder
}Borders;


/** Konstanten fuer Farben */
static const CGColor3f LOW_DARK_BLUE = {0.15f, 0.15f, 0.44f};
static const CGColor3f MEDIUM_GREEN = {0.0f, 1.0f, 0.0f};
static const CGColor3f HIGH_RED = {1.0f, 0.0f, 0.0f};
static const CGColor3f COLOR_YELLOW = {1.0f, 1.0f, 0.0f};
static const CGColor3f COLOR_WHITE = {1.0f, 1.0f, 1.0f};
static const CGColor4f COLOR_TRANS_GREEN = {0.48f, 0.98f, 0.0f, 0.75f};
static const CGColor4f COLOR_TRANS_BLACK= {0.0, 0.0f, 0.0f, 0.4f};

#endif
