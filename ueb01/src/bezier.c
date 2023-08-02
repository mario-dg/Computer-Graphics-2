/**
 * @file
 * Bezier-Modul.
 * Das Modul kapselt die Bezier-Funktionalitaet des Programms.
 *
 * @author Mario Da Graca, Christopher Ploog
 */


#include "bezier.h"
#include "stdlib.h"
#include "math.h"

/**
 * Berechnet die Position von Punkten auf einem B-Spline
 *
 * @param steps Anzahl an Schritten zwischen KontrollPunkten
 * @param controlArray uebergebene Kontrollpunkte
 * @param m Anzahl Kontrollpunkte
 * @return berechnete Position der Kurvenpunkte
 */
CGPoint2f *bezier(GLint steps, CGPoint2f *controlArray, GLint m) {

    GLint count = 0;
    CGPoint2f *curveArray = malloc(sizeof(CGPoint2f) * ((m - 3) * steps) + 1);

    //Ueber Kontrollpunkte laufen
    for (int i = 0; i < m - 3; ++i) {
        //Punkte zwischen Kontrollpunkten berechnen
        for (int j = 0; j < steps; ++j) {
            //Position auf dem Abschnitt (zwischen 0 - ~1)
            GLfloat t = (float) j / (float) steps;

            //Berechnung der X und Y-Position des Punktes
            GLfloat Px = pow((1 - t), 3) * controlArray[i][CCX] + 3 * t * pow((1 - t), 2) * controlArray[i + 1][CCX]
                         + 3 * pow(t, 2) * (1 - t) * controlArray[i + 2][CCX] + pow(t, 3) * controlArray[i + 3][CCX];
            GLfloat Py = pow((1 - t), 3) * controlArray[i][CCY] + 3 * t * pow((1 - t), 2) * controlArray[i + 1][CCY]
                         + 3 * pow(t, 2) * (1 - t) * controlArray[i + 2][CCY] + pow(t, 3) * controlArray[i + 3][CCY];
            //Punkt in Array eintragen
            curveArray[count][CCX] = Px;
            curveArray[count][CCY] = Py;
            count++;
        }
        //Eintragen des letzen Punktes
        curveArray[count][CCX] = controlArray[m - 1][CCX];
        curveArray[count][CCY] = controlArray[m - 1][CCY];
    }
    return curveArray;
}