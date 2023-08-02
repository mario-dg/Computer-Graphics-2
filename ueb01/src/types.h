/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
 * @authors Mario Da Graca, Christopher Ploog
 */

#ifndef UEB02_TYPES_H
#define UEB02_TYPES_H

#include <GL/gl.h>

#define CCX (0)
#define CCY (1)
#define CX (2)
#define CY (3)
#define CNX (4)
#define CNY (5)
#define CHX (6)
#define CHY (7)

typedef GLfloat vertex[8];

#define AMOUNT_VERT_1 (4)
#define DEFAULT_SPACING (0.15f)
#define MIN_STEPS (2)
#define MAX_STEPS (50)
#define DEFAULT_MAX_STEPS (25)
#define CIRCLE_RADIUS_CONTROL (0.05f)
#define EPSILON (0.0000001f)

struct stack_frame_s {
    struct stack_frame_s *next;
    void *data;
};

struct stack_s {
    struct stack_frame_s *top;
};

typedef struct CGPoint {
    GLfloat x,y;
}CGPoint;



/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];
typedef GLfloat CGPoint3f[3];
typedef GLint CGPoint2i[2];
/** Ausmasse eines Rechtecks (Breite/Hoehe) */
typedef GLint CGDimensions2i[2];

#endif //UEB02_TYPES_H
