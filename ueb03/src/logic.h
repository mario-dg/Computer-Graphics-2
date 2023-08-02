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
 * die Hoehen und Geschwindigkeiten der Waasersaeulen auf 0 setzen 
 */
void initLogic(void);

void calcInterpolBall(double interval);

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
 * Liefert die Anzahl der Kontrollpunkte pro Reihe/Spalte
 * @return Anzahl
 */
GLint getAmountVerticesSide();

/**
 * Liefert die Anzahl der interpolierten Punkte pro Reihe/Spalte
 * @return Anzahl
 */
GLint getAmountSplineVertices();

/**
 * Aktualisiert den ausgewaeltem Vert
 * @param i in welche Richtung verschoben wird
 */
void updateSelectedVert(int i);

/**
 * Aktualisiert die Anzahl der Kontroll Vertizes
 * @param +1/-1
 */
void updateAmountVertices(GLint v);

/**
 * Aktualisiert die Anzahl der Spline Vertizes
 * @param +1/-1
 */
void updateAmountSplineVertices(GLint v);

/**
 * liefert alle Baelle
 * @return Zeiger auf alle Baelle
 */
ball* getAllBalls();

/**
 * Liefert die Anzahl der Baelle
 * @return Anzahl der Baelle
 */
GLint getAmountBalls();

/**
 * Liefert alle KontrollVertizes
 * @return alle KontrollVertizes
 */
controlVertex *getControlVertices();

/**
 * Liefert alle SplineVertizes
 * @return alle SplineVertizes
 */
splineVertex *getSplineVertices();

/**
 * Markiert den ausgewaehlten Kontroll Vertex
 */
void updateColorSelectedControlVert();

/**
 * Laesst die Baelle losrollen und stoppt sie
 */
void toggleStartBalls();

/**
 * Berechnet die interpolierte Position der Baelle auf der Splineflaeche
 * @param calcX berechnet x
 * @param calcY berechnet y
 * @param calcZ berechnet z
 */
void calcInterpolBallPos(GLboolean calcX, GLboolean calcY, GLboolean calcZ);

/**
 * Liefert die Position der Zielsphaere
 * @return Position der Zielsphaere
 */
CGPoint3f* getGoalSphere(void);

/**
 * Startet das Spiel neu
 */
void resetGame();

/**
 * Beschleunigt den ersten noch im Spiel befindlichen Ball in eine
 * zufaellige x-z Richtung
 */
void kickFirstBall();

/**
 * Aktualisiert die Position der Zielsphaere
 */
void updateGoal();

/**
 * Liefert alle schwarzen Loecher
 * @return alle schwarzen Loecher
 */
blackHole *getAllBlackHoles();

/**
 * Liefert die Anzahl der schwarzen Loecher
 * @return Anzahl der schwarzen Loecher
 */
GLint getAmountBlackHoles();

/**
 * Aktualisiert die schwarzen Loecher
 * @param v +1/-1
 */
void updateBlackHoles(GLint v);

/**
 * Schaltet die Siegmoeglichkeit um
 */
void toggleWinning();

/**
 * Liefert den Radius eines Balls
 * @return Radius eines Balls
 */
GLfloat getBallRadius();

/**
 * Updatet die Hoehe der Schwarzen Loecher
 */
void updateBlackHoleHeight();
#endif