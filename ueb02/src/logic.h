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
 * @author Mario da Graca, Christopher Ploog
 */

/* ---- Eigene Header einbinden ---- */
#include "types.h"

/**
 * initialisiert die Logik: 
 * die Hoehen und Geschwindigkeiten der Waasersaeulen auf 0 setzen 
 */
void initLogic(void);

/**
 * Aktualisiert die Logik, wenn das Mesh vergroebert oder verfeinert wird 
 * @param state Ob, das Mesh vergoebert oder verfeinert wird 
 */
void updateLogic(expandShrinkVertices state);

/**
 * Aktualisiert die Hoehe eines Punktes, wenn dieser gepickt wird
 * @param index Index des Punktes, der gepickt wurde
 * @param direction Richtung, in die der Punkt bewegt werden soll
 */
void pickedVertex(GLuint index, int direction);

/**
 * gibt den in der Logik fuer das Hoehen- und Geschwindigkeitsarray reservierten Speicher wieder frei 
 */ 
void freeAllocatedMemLogic(void);

/**
 * Liefert den Status der Lichtberechnung.
 * @return Status der Lichtberechnung (an/aus).
 */
int getLightingState(void);

/**
 * Setzt den Status der Lichtberechnung.
 * @param status Status der Lichtberechnung (an/aus).
 */
void setLightingState(int status);

/**
 * Liefert den Status der ersten Lichtquelle.
 * @return Status der ersten Lichtquelle (an/aus).
 */
int getLight0State(void);

/**
 * Setzt den Status der ersten Lichtquelle.
 * @param status Status der ersten Lichtquelle (an/aus).
 */
void setLight0State(int status);

/**
 * Berechnet aktuellen Rotationswinkel der zweiten Lichtquelle.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void calcLight1Rotation(double interval);

/**
 * Liefert den aktuellen Rotationswinkel der zweiten Lichtquelle.
 * @return aktueller Rotationswinkel der zweiten Lichtquelle.
 */
float getLight1Rotation(void);

/**
 * liefert das Hoehenarray
 * @return Zeiger auf die Hoehen
 */
GLfloat *getHeights(void);

#endif