/**
 * @file
 * Spline-Modul.
 * Implementierung der Spline-Funktionalitaet
 *
 * @author Mario Da Graca, Christopher Ploog
 */

#ifndef UEB01_SPLINE_H
#define UEB01_SPLINE_H

#include <GL/gl.h>
#include "types.h"
#include <stdlib.h>

CGPoint2f* spline(GLint steps, CGPoint2f *controlArray, GLint m);

#endif //UEB01_SPLINE_H
