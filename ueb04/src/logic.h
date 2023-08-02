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
#include "types.h"

/**
 * initialisiert die Logik: 
 *  initialisiert die Partikel und seeded Randome Generator
 */
void initLogic(void);

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
 * Aendert die Menge der Partikel und initialisiert diese
 * @param change -1/1 verkleiner oder vergroessern
 */
void updateParticles(GLint change);

/**
 * Bewegt Ziele interpoliert
 * @param interval seit letztem Frame vergangene Zeit
 */
void moveGoals(GLfloat interval);

/**
 * Berechnet die Bewegung der Partikel
 * @param interval seit letztem Frame vergangene Zeit
 */
void calcParticleMove(GLfloat interval);

/**
 * Wechseln des FolloModes zwischen Ziele/Partikel/(Zentrum aller Partikel)
 */
void switchFollowMode(void);

/**
 * Wechseln des Ziel-Partikels
 */
void switchTargetPart(void);

/**
 * Liefert Nummer des derzeit ausgewaehltes Partikels
 * @return Ausgewaehltes Partikel
 */
GLint getSelectedParticle(void);

/**
 * Liefert Anzahl an Partikeln
 * @return Partikel Anzahl
 */
GLint getParticleCount(void);

/**
 * Liefert einen Pointer auf alle Partikel
 * @return Pointer auf Partikel
 */
particle *getAllParticles(void);

/**
 * Liefert einen Pointer auf alle Ziele
 * @return Pointer auf alle Ziele
 */
CGVector3f *getAllGoals(void);

/**
 * Liefert Anzahl an Zielen
 * @return Anzahl Ziele
 */
GLint getGoalCount(void);

/**
 * Liefert FollowMode der Partikel
 * @return FollowMode der partikel
 */
followMode getFollowMode(void);

/**
 * Aktiviert-/Deaktiviert das Bewegen der Ziele
 */
void toggleMoveGoals(void);

#endif