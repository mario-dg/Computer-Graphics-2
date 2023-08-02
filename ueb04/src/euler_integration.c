/**
 * @file
 * Euler-Integrations-Modul.
 * Das Modul kapselt die Euler-Integration. Welche die Ball Collisionen
 * miteinander und mit den Wänden kapselt. Außerdem werden Berrechnungen
 * fuer die Schwarzen Loecher gemacht.
 *
 * @author Christopher Ploog, Mario da Graca
 */

#include "euler_integration.h"
#include "math.h"

//Gauss-Konstante
#define GAUSS (50)

//Euler Zahl
#define EULER (2.7182818284f)

/**
 * Quadratischer Abstand
 * @param p1 Punkt 1
 * @param p2 Punkt 2
 * @return Abstand
 */
GLfloat squareDist(CGPoint3f p1, CGPoint3f p2) {
    return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2) + pow(p1[2] - p2[2], 2));
}

/**
 * Berechnet das Kreuzprodukt zweier uebergebener Vektoren
 * @param a Vektor A
 * @param b Vektor B
 * @param result Ergebnis
 */
void crossProduct(const CGVector3f a, const CGVector3f b, CGVector3f result) {
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];

    //Normalisieren der Laenge der Normalen
    GLfloat len = sqrt(result[0] * result[0] + result[1] * result[1] + result[2] * result[2]);
    result[0] /= len;
    result[1] /= len;
    result[2] /= len;
}

/**
 * Berrechnet den Beschleunigungs Vektor des Partikels mittels Zielen nach gauss
 * @param result Ergebniss
 * @param currPos derzeitige Position
 * @param targets Pointer auf Ziele
 * @param targetCnt Menge von Zielen
 * @param kWeak Konstante Kraft
 */
void calcVecA(CGVector3f result, CGVector3f currPos, CGVector3f *targets, GLint targetCnt, const GLfloat kWeak) {
    //Variable, auf die die Zwischenergebnisse geschrieben werden
    CGVector3f totalRes;
    totalRes[0] = 0.0f;
    totalRes[1] = 0.0f;
    totalRes[2] = 0.0f;
    //Zusammenfassen allerGewichtungen der Ziele
    for (int i = 0; i < targetCnt; ++i) {
        CGVector3f dir;
        dir[0] = targets[i][0] - currPos[0];
        dir[1] = targets[i][1] - currPos[1];
        dir[2] = targets[i][2] - currPos[2];

        GLfloat dist = squareDist(targets[i], currPos);
        CGVector3f currVecA;
        for (int j = 0; j < 3; ++j) {
            currVecA[j] = (dir[j] / dist) * kWeak;
        }
        GLfloat currWeight;
        currWeight = powf(EULER, -(powf(dist, 2.0f) / GAUSS));
        totalRes[0] += currWeight * currVecA[0];
        totalRes[1] += currWeight * currVecA[1];
        totalRes[2] += currWeight * currVecA[2];
    }
    //Ergebniss schreiben
    result[0] = totalRes[0];
    result[1] = totalRes[1];
    result[2] = totalRes[2];
}

/**
 * Berechnet die Beschwindigkeit mittels vergangener Zeit,Beschleunigung und der alten Geschwindigkeit
 * @param oldVecV alter Geschwindigkeits Vektor
 * @param dT vergangene Zeit
 * @param vecA Beschleunigungs Vektor
 * @param kVel Konstante Geschwindigkeit
 * @param result Ergebniss
 */
void calcVecV(const CGVector3f oldVecV, GLfloat dT, const CGVector3f vecA, const GLfloat kVel, CGVector3f result) {
    CGVector3f eulerV;
    eulerV[0] = oldVecV[0] + dT * vecA[0];
    eulerV[1] = oldVecV[1] + dT * vecA[1];
    eulerV[2] = oldVecV[2] + dT * vecA[2];

    GLfloat dist = sqrt(pow(eulerV[0], 2) + pow(eulerV[1], 2) + pow(eulerV[2], 2));

    for (int i = 0; i < 3; ++i) {
        result[i] = (eulerV[i] / dist) * kVel;
    }
}

/**
 * Ort der Kugel
 * @param oldVecS alter Ort
 * @param dT Zeitintervall zwischen zwei Frames
 * @param vecV Geschwindigkeit der Kugel
 * @param result Vektor S
 */
void calcVecS(const CGVector3f oldVecS, GLfloat dT, const CGVector3f vecV, CGVector3f result) {
    result[0] = oldVecS[0] + dT * vecV[0];
    result[1] = oldVecS[1] + dT * vecV[1];
    result[2] = oldVecS[2] + dT * vecV[2];
}

/**
 * Up-Vektor des Partikels
 * @param result resultierender Up-Vektor
 * @param vecV Geschwindigkeits-Vektor
 * @param vecA Beschleunigungs-Vektor
 */
void calcVecUp(CGVector3f result, CGVector3f vecV, CGVector3f vecA) {
    crossProduct(vecV, vecA, result);
    crossProduct(result, vecV, result);
}

void calcEuler(particle *currPart, CGVector3f *targets, GLint targetCnt, GLfloat interval,
               GLfloat offset) {

    //Beschleunigung
    calcVecA(currPart->acc, currPart->center, targets, targetCnt, currPart->kWeak);

    GLfloat tempKVel = currPart->kVel * offset;

    //Geschwindigkeit
    calcVecV(currPart->vel, interval, currPart->acc, tempKVel, currPart->vel);

    //Neuer Ort
    calcVecS(currPart->center, interval, currPart->vel, currPart->center);

    //Up-Vektor
    calcVecUp(currPart->up, currPart->vel, currPart->acc);
}
