/**
 * @file
 * Convex-Hull-Modul.
 * Das Modul kapselt die Funktionen, welche fuer die Berechnung der
 * Konvexen Huelle gebraucht werden.
 *
 * @author Mario Da Graca, Christopher Ploog
 */

#ifndef UEB01_CONVEXHULL_H
#define UEB01_CONVEXHULL_H

#include "stack.h"

CGPoint2f *convexHull(CGPoint points[], GLint n, GLint *amntConvexHullVerts);

#endif //UEB01_CONVEXHULL_H