/**
 * @file
 * Ein-/Ausgabe-Modul.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.
 *
 * @author Mario Da Graca, Christopher Ploog
 */

#ifndef UEB02_IO_H
#define UEB02_IO_H

/* ---- System Header einbinden ---- */
#include <GL/gl.h>

/* ---- Eigene Header einbinden ---- */
#include "types.h"

int initAndStartIO(char *title, int width, int height);

#endif //UEB02_IO_H
