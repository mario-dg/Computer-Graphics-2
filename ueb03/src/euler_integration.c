/**
 * @file
 * Euler-Integrations-Modul.
 * Das Modul kapselt die Euler-Integration. Welche die Ball Collisionen
 * miteinander und mit den Wänden kapselt. Außerdem werden Berrechnungen
 * fuer die Schwarzen Loecher gemacht.
 *
 * @author Christopher Ploog, Mario da Graca
 */

#include <stdlib.h>
#include <stdio.h>
#include "euler_integration.h"
#include "math.h"
#include "logic.h"
//Gravitation
#define g (3.81f)
//Masse ball
#define m (2.0f)
//Reibung
#define Fr (0.99f)
//Federkonstante Wand
#define k (500.0f)
//Federkonstante Kugel
#define kk (50.0f)
//Anzhiehungskraft schwarzes Loch
#define blackHoleAtt (5.0f)
//Gravitationsvektor
CGVector3f vec_g = {0, -g, 0};

/**
 * Bildet das Skalarprodukt aus 2 Vektoren
 * @param a Vektor 1
 * @param b Vektor 2
 * @return Skalarprodukt
 */
GLfloat scalarProduct(const CGVector3f a, const CGVector3f b){
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

/**
 * Gegenvektor zu n
 * @param n Normale der Flaeche
 * @param result Vektor l
 */
void calcVecL(CGVector3f n, CGVector3f result){
    GLfloat temp = scalarProduct(vec_g, n);
    result[0] = temp * n[0];
    result[1] = temp * n[1];
    result[2] = temp * n[2];
}

/**
 * Rollrichtung der Kugel
 * @param vecL Vektor L
 * @param result Vektor F
 */
void calcVecF(const CGVector3f vecL, CGVector3f result){
    result[0] = vec_g[0] - vecL[0];
    result[1] = vec_g[1] - vecL[1];
    result[2] = vec_g[2] - vecL[2];
}

/**
 * Beschlenigung der Kugel
 * @param vecF Rollrichtung der Kugel
 * @param result Vektor A
 */
void calcVecA(const CGVector3f vecF, CGVector3f result){
    result[0] = vecF[0] / m;
    result[1] = vecF[1] / m;
    result[2] = vecF[2] / m;
}

/**
 * Geschwindigkeit der Kugel
 * @param oldVecV alte Geschwindigkeit
 * @param dT Zeitintervall zwischen zwei Frames
 * @param vecA Beschleunigung der Kugel
 * @param result Vektor V
 */
void calcVecV(const CGVector3f oldVecV, GLfloat dT, const CGVector3f vecA, CGVector3f result){
    result[0] = oldVecV[0] + dT * vecA[0];
    result[1] = oldVecV[1] + dT * vecA[1];
    result[2] = oldVecV[2] + dT * vecA[2];
}

/**
 * Ort der Kugel
 * @param oldVecS alter Ort
 * @param dT Zeitintervall zwischen zwei Frames
 * @param vecV Geschwindigkeit der Kugel
 * @param result Vektor S
 */
void calcVecS(const CGVector3f oldVecS, GLfloat dT, const CGVector3f vecV, CGVector3f result){
    result[0] = oldVecS[0] + dT * vecV[0];
    result[1] = oldVecS[1] + dT * vecV[1];
    result[2] = oldVecS[2] + dT * vecV[2];
}

/**
 * Quadratischer Abstand
 * @param p1 Punkt 1
 * @param p2 Punkt 2
 * @return Abstand
 */
GLfloat squareDist(CGPoint3f p1, CGPoint3f p2) {
    return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[2] - p2[2], 2));
}

/**
 * Berrechnung der Kraft nach Penalty-Methode
 * @param currBall Zeiger auf aktuellen Ball
 * @param normal Normale
 * @param d Eingedrungende Distanz
 * @param feather Federkonstante
 */
void penaltyMethod(ball* currBall, CGVector3f normal, GLfloat d, GLfloat feather){
    //Penalty-Kraft
    GLfloat Fpen = feather * d;
    //Penalty-Kraft-Vektor
    CGVector3f vecFpen;
    vecFpen[CX] = Fpen * normal[CX];
    vecFpen[CY] = Fpen * normal[CY];
    vecFpen[CZ] = Fpen * normal[CZ];
    //Gegenbeschleunigung
    CGVector3f vecA;
    vecA[CX] = vecFpen[CX] / m;
    vecA[CY] = vecFpen[CY] / m;
    vecA[CZ] = vecFpen[CZ] / m;

    //Wird auf Beschleunigung des Balles addiert
    currBall->acc[CX] += vecA[CX];
    currBall->acc[CY] += vecA[CY];
    currBall->acc[CZ] += vecA[CZ];
}

/**
 * Kollisionreaktion anhand der Penaltymethode
 * @param currBall Aktueller Ball
 * @param borderPos Koordinaten-Wert der kollidierten Wand
 * @param border enum mit welcher Wand kollidiert wurde
 */
void calcPenaltyCollision(ball* currBall, GLfloat borderPos, Borders border){
    //Normale der Wand
    CGVector3f normal = {0.0f, 0.0f, 0.0f};
    //Abstand des am weitesten eingedrungenden Punktes vom Rand
    GLfloat d = 0.0f;
    switch (border) {
        case leftBorder:
            normal[CX] = 1.0f;
            d = fabsf(currBall->center[CX] - getBallRadius() - borderPos);
            break;
        case rightBorder:
            normal[CX] = -1.0f;
            d = fabsf(currBall->center[CX] + getBallRadius() - borderPos);
            break;
        case upperBorder:
            normal[CZ] = 1.0f;
            d = fabsf(currBall->center[CZ] - getBallRadius() - borderPos);
            break;
        case lowerBorder:
            normal[CZ] = -1.0f;
            d = fabsf(currBall->center[CZ] + getBallRadius() - borderPos);
            break;
    }
    penaltyMethod(currBall, normal, d, k);
}



/**
 * Prueft Kollision mit einer Wand
 * @param currBall Aktueller Ball
 * @param rBorder Koordinaten-Wert der rechten Wand
 * @param lBorder Koordinaten-Wert der linken Wand
 * @param uBorder Koordinaten-Wert der oberen Wand
 * @param loBorder Koordinaten-Wert der untere Wand
 */
void limitBallPos(ball* currBall, GLfloat rBorder, GLfloat lBorder, GLfloat uBorder, GLfloat loBorder) {
    //Abfrage und Behandlung bei Collision mit einer Wand
        if (currBall->center[0] + getBallRadius() > rBorder) {
            calcPenaltyCollision(currBall, rBorder, rightBorder);

        } else if (currBall->center[0] - getBallRadius() < lBorder) {
            calcPenaltyCollision(currBall, lBorder, leftBorder);
        }
        if (currBall->center[2] + getBallRadius() > loBorder) {
            calcPenaltyCollision(currBall, loBorder, lowerBorder);
        } else if (currBall->center[2] - getBallRadius() < uBorder) {
            calcPenaltyCollision(currBall, uBorder, upperBorder);
        }
}

/**
 * Behandlung der Collision von zwei Baellen
 * @param currBall derzeitiger Ball
 * @param collidedBall Ball mit dem Kollidiert wurde
 */
void handleBallCollision(ball* currBall, ball collidedBall) {
    CGVector3f normal = {currBall->center[CX] - collidedBall.center[CX],
                         currBall->center[CY] - collidedBall.center[CY],
                         currBall->center[CZ] - collidedBall.center[CZ]};
    //eingedrungende Tiefe berechnen
    GLfloat d = sqrt(normal[CX] * normal[CX] + normal[CY] * normal[CY] + normal[CZ] * normal[CZ]);
    normal[CX] /= d;
    normal[CY] /= d;
    normal[CZ] /= d;

    //Berechnung und Anwendung der Kraft
    penaltyMethod(currBall, normal, d, kk);
}

/**
 * Funktion um die Collision von zwei Baellen zu erkennen
 * @param currBall
 * @param allBalls
 * @param amountBalls
 */
void ballCollision(ball* currBall, ball* allBalls, GLint amountBalls){
    //Laufen ueber alle Baelle
    for (int i = 0; i < amountBalls; ++i) {
        //Berechnung nur mit anderen Baellen, nicht sich selbst
        if(currBall->id != i) {
            ball collidedBall = allBalls[i];
            //Collision nur mit Aktiven Baellen
            if(collidedBall.active) {
                GLfloat d = squareDist(currBall->center, collidedBall.center);
                //Bei zu geringen Abstand, Collision berechnen
                if (d < (2 * getBallRadius())) {
                    handleBallCollision(currBall, collidedBall);
                }
            }
        }
    }
}


/**
 * Berechnet die Anziehungsbeschleunigung der schwarzen Loecher
 * @param currBall Aktueller Ball
 * @param blackHoles Alle schwarzen Loecher
 * @param amountBH Anzahl der schwarzen Loecher
 */
void calcBlackHoleAttraction(ball* currBall, blackHole* blackHoles, GLint amountBH){
    for (int i = 0; i < amountBH; ++i) {
        //Aktuelles schwarzes Loch
        blackHole currBlackHole = blackHoles[i];
        //Abstand vom aktuellen Ball zum aktuellen schwarzen Loch
        GLfloat d = squareDist(currBall->center, currBlackHole.center);
        //Ball betritt aeusseren Radius des schwarzen Lochs und wird angezogen
        if(d <= currBlackHole.outerRadius){
            //Anziehungsrichtung
            CGVector3f dir = {currBlackHole.center[CX] - currBall->center[CX],
                              currBlackHole.center[CY] - currBall->center[CY],
                              currBlackHole.center[CZ] - currBall->center[CZ]};
            //Wird auf Beschleunigung des Balles addiert
            currBall->acc[CX] += blackHoleAtt * dir[CX];
            currBall->acc[CY] += blackHoleAtt * dir[CY];
            currBall->acc[CZ] += blackHoleAtt * dir[CZ];

        }
        //Ball betritt inneren Radius des schwarzen Lochs -> wird "verschluckt"
        if(d <= currBlackHole.innerRadius){
            currBall->active = GL_FALSE;
        }
    }
}

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
                      ball* allBalls, GLint amountBalls){
    //Berechnen der notwendigen Vektoren
    CGVector3f vecL;
    calcVecL(normal, vecL);
    CGVector3f vecF;
    calcVecF(vecL, vecF);
    CGVector3f vecA;
    calcVecA(vecF, vecA);

    //Beschleunigung uebernehmen
    currBall->acc[0] = vecA[0];
    currBall->acc[1] = vecA[1];
    currBall->acc[2] = vecA[2];

    //Moegliche Ball-Collision behandeln
    ballCollision(currBall, allBalls, amountBalls);

    //Kraft ausgehend eines Schwarzen Loches berechnen
    calcBlackHoleAttraction(currBall, blackHoles, amountBH);

    //Ball-Position limitieren
    limitBallPos(currBall, rightBorder, leftBorder, upperBorder, lowerBorder);

    //Geschwindigkeit des Balles berechnen und mit der Reibung multiplizieren
    CGVector3f vecV;
    calcVecV(currBall->vel, interval, currBall->acc, vecV);
    currBall->vel[0] = vecV[0] * Fr;
    currBall->vel[1] = vecV[1] * Fr;
    currBall->vel[2] = vecV[2] * Fr;

    //Neuer Ort der Kugel berechnen und uebernehmen
    CGVector3f vecS;
    calcVecS(currBall->center, interval, vecV, vecS);
    currBall->center[0] = vecS[0];
    currBall->center[1] = vecS[1];
    currBall->center[2] = vecS[2];
}