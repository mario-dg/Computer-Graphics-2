/**
 * @file
 * Spline-Surface-Modul.
 * Das Modul kapselt die Berechnungen, fuer die Erstellung einer Flaeche
 * aus Spline-Kurven.
 *
 * @author Christopher Ploog, Mario Da Graca
 */

#include <stdio.h>
#include <stdlib.h>
#include "spline_surface.h"
#include "math.h"


//Spline-Interpolations-Matrix (konstant)
CGMatrix44f splineInterMat = {{-1, 3,  -3, 1},
                              {3,  -6, 3,  0},
                              {-3, 0,  3,  0},
                              {1,  4,  1,  0}};

//Transponierte Spline-Interpolations-Matrix (konstant)
CGMatrix44f transSplineInterMat = {{-1, 3,  -3, 1},
                                   {3,  -6, 0,  4},
                                   {-3, 3,  3,  1},
                                   {1,  0,  0,  0}};

/**
 * Multiplikation zweier 4x4 Matrizen
 * @param a 4x4 Matrix
 * @param b 4x4 Matrix
 * @param result 4x4 Matrix
 */
static void multiply44x44(CGMatrix44f a, CGMatrix44f b, CGMatrix44f result) {
    for (int i = 0; i < 4; ++i) {
        for (int k = 0; k < 4; ++k) {
            GLfloat sum = 0.0f;
            for (int x = 0; x < 4; ++x) {
                sum += a[i][x] * b[x][k];
            }
            result[i][k] = sum;
        }
    }
}


/**
 * Multiplikation einer 4x4 Matrix mit einem 4x1 Vektor
 * @param a 4x4 Matrix
 * @param b 4x1 Vektor
 * @param result 4x1 Vektor
 */
static void multiply44x41(CGMatrix44f a, CGMatrix41f b, CGMatrix41f result) {
    for (int k = 0; k < 4; ++k) {
        GLfloat sum = 0.0f;
        for (int x = 0; x < 4; ++x) {
            sum += a[k][x] * b[x][0];
        }
        result[k][0] = sum;
    }
}

/**
 * Mulitplikation eines Vektors mit Dimension 1x4 und 4x1
 * @param a 1x4-Vektor
 * @param b 4x1-Vektore
 * @param result Skalares Ergebnis
 */
static void multiply14x41(CGMatrix14f a, CGMatrix41f b, GLfloat *result) {
    GLfloat sum = 0.0f;
    for (int k = 0; k < 4; ++k) {
        sum += a[0][k] * b[k][0];
    }
    *result = sum;
}

/**
 * Bestimmt welches Polynom verwendet werden soll
 * @param N bigT oder bigS
 * @param[in/out] n lokales t oder lokales s
 * @param amountPolynomsRow Anzahl der Polynome pro Reihe/Spalte
 * @return Index des Polynoms
 */
GLint getSection(GLfloat N, GLfloat* n, GLint amountPolynomsRow){
    GLint temp = N * amountPolynomsRow;
    *n = N * amountPolynomsRow - temp;
    //Sonderfall -> N = 1
    if(fabsf(N - 1.0f) <= EPSILON){
        temp--;
        *n = 1.0f;
    }
    return temp;
}

/**
 * Fuehrt die Matrixmultiplikation aus, in denen nicht der t- und s-Monomvektor vorkommen
 * @param verts Vertizes um die Geometriematrix zu erstellen
 * @param tSection Gibt an welches Polynom verwendet werden soll
 * @param sSection Gibt an welches Polynom verwendet werden soll
 * @param amountControlVertices Anzahl der Kontrollvertizes
 * @param result Ergebnis
 */
void matrixMultiplication(GLfloat* verts, GLint tSection, GLint sSection, GLint amountControlVertices, CGMatrix44f result){
    //Geometrie-Matrix
    CGMatrix44f geomMat;
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            geomMat[x][y] = verts[((x + sSection) * amountControlVertices) + y + tSection];
        }
    }

    //Geometrie-Matrix x transponierte Spline-Interpolationsmatrix
    CGMatrix44f geomMatTransSpline;
    multiply44x44(geomMat, transSplineInterMat, geomMatTransSpline);
    //Spline-Interpolationsmatrix x vorheriges Ergebnis
    multiply44x44(splineInterMat, geomMatTransSpline, result);
}


/**
 * Berechnet die x/y/z Koordinate eines Punktes
 * @param s linear abgebildete y-Koordinate
 * @param t linear abgebildete x-Koordinate
 * @param temp Spline * Geom * trans Spline
 * @param mode Interpolation oder Ableitung
 * @return Koordinatenwert
 */
GLfloat calcSplineCoords(GLfloat s, GLfloat t, CGMatrix44f temp, calculationMode mode){
    CGMatrix14f sMonom = {{s*s*s, s*s, s, 1}};
    CGMatrix41f tMonom = {{t*t*t},
                          {t*t},
                          {t},
                          {1}};
    switch (mode) {
        case interpolation:
            break;
        case derivative_s:
            //Partielle Ableitung in s
            sMonom[0][0] = 3*s*s;
            sMonom[0][1] = 2*s;
            sMonom[0][2] = 1;
            sMonom[0][3] = 0;
            break;
        case derivative_t:
            //Partielle Ableitung in t
            tMonom[0][0] = 3*t*t;
            tMonom[1][0] = 2*t;
            tMonom[2][0] = 1;
            tMonom[3][0] = 0;
            break;
    }
    //Multiplikation mit dem t-Monomvektor
    CGMatrix41f rightSide;
    multiply44x41(temp, tMonom, rightSide);

    //Multiplikation mit dem s-Monomvektor -> Ergebnis Skalar
    GLfloat a;
    multiply14x41(sMonom, rightSide, &a);
    return a / 36.0f;
}

/**
 * Berechnet die Punkte für eine interpolierte Splinefläche
 * @param steps Genauigkeit der Splinefläche
 * @param controlArray Array mit allen Kontrollpunkten
 * @param m Anzahl der Kontrollpunkte pro Reihe
 * @return Koordinaten der Splineflächenpunkte
 */
GLfloat *splineSurface(GLint amountControlVertices, GLint amountInterpolVertices, GLfloat *verts, calculationMode mode) {
    GLfloat *result = calloc(SQUARE(amountInterpolVertices), sizeof(GLfloat));
    GLfloat bigT = 0.0f;
    GLfloat bigS = 0.0f;

    //Anzahl der Polynome in dem mesh
    GLint amountPolynomsRow = amountControlVertices - 3;
    //Kleinschrittigkeit von bigT und bigS
    GLfloat step = 1.0f / (float)(amountInterpolVertices - 1);
    GLint index = 0;

    for (int i = 0; i < amountInterpolVertices; ++i) {
        if(i == amountInterpolVertices - 1){
            bigT = 1.0f;
        }
        //bigT linear abbilden, um lokales t fuer Berechnungen zu bekommen
        GLfloat t;
        GLint Tsection = getSection(bigT, &t, amountPolynomsRow);


        for (int j = 0; j < amountInterpolVertices; ++j) {
            if(j == amountInterpolVertices - 1){
                bigS = 1.0f;
            }
            //bigS linear abbilden, um lokales s fuer Berechnungen zu bekommen
            GLfloat s;
            GLint Ssection = getSection(bigS, &s, amountPolynomsRow);

            //Spline * Geom * trans Spline
            CGMatrix44f temp;
            matrixMultiplication(verts, Tsection, Ssection, amountControlVertices, temp);

            GLfloat a = calcSplineCoords(s, t, temp, mode);
            result[index++] = a;
            bigS += step;
        }
        bigS = 0.0f;
        bigT += step;
    }
    return result;
}