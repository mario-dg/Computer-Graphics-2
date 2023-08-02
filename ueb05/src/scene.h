#ifndef __SCENE_H__
#define __SCENE_H__
#include "utils.h"
/**
 * @file
 * Schnittstelle des Darstellungs-Moduls.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 *
 * @author Mario da Graca, Leonhard Brandes
 */

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar
 */
void drawScene (void);


/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene (void);

#endif
