#ifndef __LOGIC_H__
#define __LOGIC_H__
/**
 * @file
 * Schnittstelle des Logik-Moduls.
 * Das Modul kapselt die Programmlogik. Wesentliche Programmlogik ist die
 * Verwaltung und Aktualisierung der Position und Farbe eines Rechtecks. Die
 * Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Christopher Ploog, Mario da Graca
 */

/* ---- Eigene Header einbinden ---- */
#include "utils.h"
#include "loadObj.h"
#include "boundingBox.h"

/**
 * initialisiert die Logik
 */
void logic_initLogic(void);

/**
 * Setzt die Kamera Position
 * @param cameraPos neue Position
 */
void logic_updateViewDir(viewMode mode);

/**
 * Liefert die Farbwerte des Ergebnisbildes
 * @return
 */
Color *logic_getFramebuffer(void);

/**
 * Gibt den reservierten Speicher wieder frei
 */
void logic_freeData(void);

/**
 * (De-)aktiviert das 1. Punktlicht und rendert die Szene neu
 */
void logic_togglePointLight1(void);

/**
 * (De-)aktiviert das 2. Punktlicht und rendert die Szene neu
 */
void logic_togglePointLight2(void);

/**
 * Wechselt zwischen keiner BoundingBox, der AABB und der OBB vom Hasen
 */
void logic_toggleBoundingBoxes(void);

/**
 * (De-)aktiviert die Anzeige der BoundingBox
 */
void logic_toggleShowBB(void);

/**
 * Waehlt den Threading Modus aus
 * @param opt Threading Modus
 */
void logic_setThreadingOptions(multiThreadOptions opt);
#endif