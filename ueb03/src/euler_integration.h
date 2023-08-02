/**
 * @file
 * Euler-Integrations-Modul.
 * Das Modul kapselt die Euler-Integration. Welche die Ball Collisionen
 * miteinander und mit den Wänden kapselt. Außerdem werden Berrechnungen
 * fuer die Schwarzen Loecher gemacht.
 *
 * @author Christopher Ploog, Mario da Graca
 */

#ifndef UEB02_EULER_INTEGRATION_H
#define UEB02_EULER_INTEGRATION_H
#include "spline_surface.h"

/**
 * Euler Integration, Kollision und Einfluesse der Umgebung
 * @param currBall Aktueller Ball
 * @param normal Normale der Splineflaeche an der Koordinate des Balles
 * @param interval Zeitintervall zwischen zwei Frames
 * @param rightBorder Koordinaten-Wert der rechten Wand
 * @param leftBorder Koordinaten-Wert der linken Wand
 * @param upperBorder Koordinaten-Wert der oberen Wand
 * @param lowerBorder Koordinaten-Wert der unteren Wand
 * @param blackHoles Alle schwarzen Loecher
 * @param amountBH Anzahl der schwarzen Loecher
 * @param allBalls Zeiger auf alle Baelle
 * @param amountBalls Anzahl der Baelle
 */
void calcContactPoint(ball *currBall, CGVector3f normal, GLfloat interval,
                      GLfloat rightBorder, GLfloat leftBorder, GLfloat upperBorder, GLfloat lowerBorder,
                      blackHole* blackHoles, GLint amountBH,
                      ball* allBalls, GLint amountBalls);

/**
 * Kollisionreaktion anhand der Penaltymethode
 * @param currBall Aktueller Ball
 * @param borderPos Koordinaten-Wert der kollidierten Wand
 * @param border enum mit welcher Wand kollidiert wurde
 */
void calcPenaltyCollision(ball* currBall, GLfloat borderPos, Borders border);

/**
 * Berechnet die Anziehungsbeschleunigung der schwarzen Loecher
 * @param currBall Aktueller Ball
 * @param blackHoles Alle schwarzen Loecher
 * @param amountBH Anzahl der schwarzen Loecher
 */
void calcBlackHoleAttraction(ball* currBall, blackHole* blackHoles, GLint amountBH);

/**
 * Quadratischer Abstand
 * @param p1 Punkt 1
 * @param p2 Punkt 2
 * @return Abstand
 */
GLfloat squareDist(CGPoint3f p1, CGPoint3f p2);
#endif //UEB02_EULER_INTEGRATION_H
