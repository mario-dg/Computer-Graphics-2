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

/* ---- Konstanten ---- */
/** Default Fenster Groesse */
#define DEFAULT_WINDOW_WIDTH 600
#define DEFAULT_WINDOW_HEIGHT 600
#define AREA_WIDTH 2

/** Anzahl der Texturen. */
#define TEX_COUNT (3)

//Konstanten und Typen zur besseren Lesbarkeit
#define X (0)
#define Y (1)
#define Z (2)

/* ---- Typedeklarationen ---- */


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


typedef enum e_randMode
{
    randParticles,
    randGoals
} randMode;

typedef enum e_followMode
{
    followGoals,
    followSingle,
    followAll
} followMode;

/** Struct um einen Partikel darzustellen*/
typedef struct particle {
    CGPoint3f center;
    CGVector3f acc;
    GLfloat kWeak; //Konstante Anziehungskraft
    GLfloat kVel;  //Konstante Geschwindigkeit
    CGVector3f vel;
    CGVector3f up;
} particle;


/** Konstanten fuer Farben */
static const CGColor3f LOW_DARK_BLUE = {0.15f, 0.15f, 0.44f};
static const CGColor3f MEDIUM_GREEN = {0.0f, 1.0f, 0.0f};
static const CGColor3f HIGH_RED = {1.0f, 0.0f, 0.0f};
static const CGColor3f COLOR_YELLOW = {1.0f, 1.0f, 0.0f};
static const CGColor3f COLOR_WHITE = {1.0f, 1.0f, 1.0f};
static const CGColor4f COLOR_TRANS_GREEN = {0.48f, 0.98f, 0.0f, 0.75f};
static const CGColor4f COLOR_TRANS_BLACK= {0.0, 0.0f, 0.0f, 0.4f};

#endif
