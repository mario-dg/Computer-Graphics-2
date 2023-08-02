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

#include "types.h"

/**
 * Berechnet die neue Beschleunigung/Geschwndigkeit/Position des uebergebenen Partikels,
 * mit Einbeziehung der Ziele nach Euler und geweichtet diese nach Gauss.
 * @param currPart aktuelles Partikel
 * @param targets Pointer auf Ziel/e
 * @param targetCnt Anzahl an Zielen
 * @param interval Zeit seit letztem Aufruf
 * @param offset Faktor fuer die Beschleunigung des Ziel-Partikels
 */
void calcEuler(particle *currPart, CGVector3f *targets, GLint targetCnt, GLfloat interval, GLfloat offset);

/**
 * Berechnet das Kreuzprodukt aus Vektor a und b und gibt es auf result aus
 * @param a erster Vektor
 * @param b zweiter Vektor
 * @param result Ergebnis
 */
void crossProduct(const CGVector3f a, const CGVector3f b, CGVector3f result);

#endif //UEB02_EULER_INTEGRATION_H
