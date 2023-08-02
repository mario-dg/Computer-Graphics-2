/**
 * @file
 * Spline-Surface-Modul.
 * Das Modul kapselt die Berechnungen, fuer die Erstellung einer Flaeche
 * aus Spline-Kurven.
 *
 * @author Christopher Ploog, Mario Da Graca
 */

#ifndef UEB02_SPLINE_SURFACE_H
#define UEB02_SPLINE_SURFACE_H
#include "types.h"

/**
 * Bestimmt welches Polynom verwendet werden soll
 * @param N bigT oder bigS
 * @param[in/out] n lokales t oder lokales s
 * @param amountPolynomsRow Anzahl der Polynome pro Reihe/Spalte
 * @return Index des Polynoms
 */
GLint getSection(GLfloat N, GLfloat* n, GLint amountPolynomsRow);

/**
 * Fuehrt die Matrixmultiplikation aus, in denen nicht der t- und s-Monomvektor vorkommen
 * @param verts Vertizes um die Geometriematrix zu erstellen
 * @param tSection Gibt an welches Polynom verwendet werden soll
 * @param sSection Gibt an welches Polynom verwendet werden soll
 * @param amountControlVertices Anzahl der Kontrollvertizes
 * @param result Ergebnis
 */
void matrixMultiplication(GLfloat* verts, GLint tSection, GLint sSection, GLint amountControlVertices, CGMatrix44f result);

/**
 * Berechnet die x/y/z Koordinate eines Punktes
 * @param s linear abgebildete y-Koordinate
 * @param t linear abgebildete x-Koordinate
 * @param temp Spline * Geom * trans Spline
 * @param mode Interpolation oder Ableitung
 * @return Koordinatenwert
 */
GLfloat calcSplineCoords(GLfloat s, GLfloat t, CGMatrix44f temp, calculationMode mode);

/**
 * Berechnet die Punkte für eine interpolierte Splinefläche
 * @param steps Genauigkeit der Splinefläche
 * @param controlArray Array mit allen Kontrollpunkten
 * @param m Anzahl der Kontrollpunkte pro Reihe
 * @return Koordinaten der Splineflächenpunkte
 */
GLfloat *splineSurface(GLint amountControlVertices, GLint amountInterpolVertices, GLfloat *verts, calculationMode mode);
#endif //UEB02_SPLINE_SURFACE_H
