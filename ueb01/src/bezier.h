/**
 * @file
 * Bezier-Modul.
 * Das Modul kapselt die Bezier-Funktionalitaet des Programms.
 *
 * @author Mario Da Graca, Christopher Ploog
 */

#ifndef UEB01_BEZIER_H
#define UEB01_BEZIER_H

#include "types.h"

CGPoint2f* bezier(GLint steps, CGPoint2f *controlArray, GLint m);

#endif //UEB01_BEZIER_H
