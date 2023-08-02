/**
 * @file
 * Spline-Modul.
 * Implementierung der Spline-Funktionalitaet
 *
 * @author Mario Da Graca, Christopher Ploog
 */

#include "spline.h"


/**
 * Gewichtungsfunktion b0
 * @param v Position auf Strecken-Stueck ( 0 - 1)
 * @return Ergebnis
 */
GLfloat b0(GLfloat v) {

    return ((1 - v) * (1 - v) * (1 - v)) / 6.0f;
}

/**
 * Gewichtungsfunktion b1
 * @param v Position auf Strecken-Stueck ( 0 - 1)
 * @return Ergebnis
 */
GLfloat b1(GLfloat v) {

    return ((3 * v * v * v) - (6 * v * v) + 4) / 6.0f;
}

/**
 * Gewichtungsfunktion b2
 * @param v Position auf Strecken-Stueck ( 0 - 1)
 * @return Ergebnis
 */
GLfloat b2(GLfloat v) {

    return ((-3 * v * v * v) + (3 * v * v) + (3 * v) + 1) / 6.0f;
}

/**
 * Gewichtungsfunktion b3
 * @param v Position auf Strecken-Stueck ( 0 - 1)
 * @return Ergebnis
 */
GLfloat b3(GLfloat v) {

    return (v * v * v) / 6.0f;
}

/**
 * Berechnung der B-Spline
 * @param steps Anzahl an Zwischenschritten zwischen Kontroll-Punkten
 * @param controlArray uebergebenes Kontroll-Punkt-Array
 * @param m Anzahl an Kontroll-Punkten
 * @return Array von Curve-Punkten
 */
CGPoint2f* spline(GLint steps, CGPoint2f *controlArray, GLint m) {
    GLint count = 0;
    CGPoint2f *curveArray = malloc(sizeof(CGPoint2f) * ((m - 3) * steps + 1));
    if(!curveArray) {
        exit(1);
    }
    //Schleife ueber Kontroll Punkte
    for (int i = 0; i < m - 3; i++) {
        //Schleife ueber Zwischenschritte
        for (int j = 0; j < steps; j++) {

            GLfloat u = (float)j / (float)steps;

            GLfloat Qx = b0(u) * controlArray[i][CCX] + b1(u) * controlArray[i + 1][CCX] +
                         b2(u) * controlArray[i + 2][CCX] + b3(u) * controlArray[i + 3][CCX];

            GLfloat Qy = b0(u) * controlArray[i][CCY] + b1(u) * controlArray[i + 1][CCY] +
                         b2(u) * controlArray[i + 2][CCY] + b3(u) * controlArray[i + 3][CCY];

            curveArray[count][CCX] = Qx;
            curveArray[count][CCY] = Qy;

            count++;
        }

        //Algorithmus berueksichtigt nicht den letzten Punkt -> Extra berechnen
        GLfloat Qx = b0(1.0f) * controlArray[m - 4][CCX] + b1(1.0f) * controlArray[m - 3][CCX] +
                     b2(1.0f) * controlArray[m - 2][CCX] + b3(1.0f) * controlArray[m - 1][CCX];

        GLfloat Qy = b0(1.0f) * controlArray[m - 4][CCY] + b1(1.0f) * controlArray[m - 3][CCY] +
                     b2(1.0f) * controlArray[m - 2][CCY] + b3(1.0f) * controlArray[m - 1][CCY];

        curveArray[count][CCX] = Qx;
        curveArray[count][CCY] = Qy;
    }
    return curveArray;
}

