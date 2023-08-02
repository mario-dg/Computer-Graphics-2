/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Logik-Funktionalitaet des Programms.
 *
 * @authors Mario Da Graca, Christopher Ploog
 */

#ifndef UEB01_LOGIC_H
#define UEB01_LOGIC_H

#include "types.h"

void init();

void moveVertex(GLint x, GLint y, GLfloat width, GLfloat height, GLint closestIndex);

GLint getAmountVertices();

GLint getAmountCurveVertices();

GLint getAmountConvexHullVertices();

GLint adjustSteps(GLint v);

CGPoint2f *getControlVertices();

CGPoint2f *getCurveVertices();

CGPoint2f *getConvexHullVertices();

GLint getClosestIndex(CGPoint2f mousePos);

void calcConvexHull();

void toggleSplineBezier();

GLint drawSplineBezier();

CGPoint2f *getNormals();

void calculateNormals();

#endif //UEB01_LOGIC_H
