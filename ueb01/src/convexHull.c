/**
 * @file
 * Convex-Hull-Modul.
 * Das Modul kapselt die Funktionen, welche fuer die Berechnung der
 * Konvexen Huelle gebraucht werden.
 *
 * @author Mario Da Graca, Christopher Ploog
 */

#include <stdio.h>
#include "convexHull.h"
#include "stdlib.h"
#include "math.h"

//Startpunkt der konvexen Huelle
CGPoint startP;

/**
 * Liefert den vorletzten Punkt des Stacks
 * @param st Stack mit Punkten
 * @return CGPoint, liefert vorletzten Punkt des Stacks
 */
CGPoint secondToTop(stack_t *st) {
    CGPoint *p = stack_pop(st);
    CGPoint *res = stack_top(st);
    stack_push(st, p);
    return *res;
}

/**
 * Tauscht zwei Punkte
 * @param p1 Punkt 1
 * @param p2 Punkt 2
 */
void swap(CGPoint *p1, CGPoint *p2) {
    struct CGPoint temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

/**
 * Berechnet die Orientierung der 3 Punkte
 * @param p Punkt 1
 * @param q Punkt 2
 * @param r Punkt 3
 * @return 0 -> p,q,r sind Kolinear
 *         1 -> im Uhrzeigersinn geordnet
 *         2 -> gegen den Uhrzeigersinn geordnet
 */
GLint orientation(const CGPoint p, const CGPoint q, const CGPoint r) {
    GLfloat val = (q.y - p.y) * (r.x - q.x) -
                  (q.x - p.x) * (r.y - q.y);
    if (fabs(val) <= EPSILON) return 0;
    return (val > 0.0f) ? 1 : 2;
}

/**
 * Berechnet die Distanz zwischen 2 Punkten
 * @param p1 Punkt 1
 * @param p2 Punkt 2
 * @return Ergebnis als Float
 */
GLfloat squareDist(const CGPoint p1, const CGPoint p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) +
           (p1.y - p2.y) * (p1.y - p2.y);
}

/**
 * Vergleichsfunktion, die von qsort() genutzt wird, um ein Array anhand des
 * Startpunktes zu sortieren
 * @param p1 Punkt 1
 * @param p2 Punkt 2
 * @return GLint Vergleichsergebnis
 */
GLint compare(const void *p1, const void *p2) {
    CGPoint *p3 = (CGPoint *) p1;
    CGPoint *p4 = (CGPoint *) p2;

    GLint orient = orientation(startP, *p3, *p4);
    if (orient == 0) {
        return (squareDist(startP, *p4) >= squareDist(startP, *p3)) ? -1 : 1;
    }
    return (orient == 2) ? -1 : 1;
}

/**
 * Berechnet die Konvexe Huelle der uebergebenen Punkte
 * @param points Punkte
 * @param n Anzahl der Punkte
 * @return CGPoint2f Konvexe Huelle
 */
CGPoint2f *convexHull(CGPoint points[], GLint n, GLint *amntConvexHullVerts) {
    GLfloat minY = points[0].y;
    GLint minIdx = 0;

    //Punkt mit niedrigstem y-Wert finden
    for (int i = 1; i < n; i++) {
        GLfloat y = points[i].y;

        //Waehle Punkt mit kleinerem y-Wert aus
        //y-Wert identisch (oder sehr aehnlich) -> Waehle Punkt mit geringerem x-Wert
        if ((y < minY) || ((fabsf(y - minY) <= EPSILON) && (points[i].x < points[minIdx].x))) {
            minY = points[i].y;
            minIdx = i;
        }
    }

    //Niedrigstem Punkt an die erste Stelle des Array tauschen
    swap(&points[0], &points[minIdx]);

    //Sortieren der restlichen Punkte anhand des ersten Punktes
    // P1 kommt vor P2 wenn P2 einen groesseren Polaren Winkel hat
    // (gegen den Uhrzeigersinn)
    startP.x = points[0].x;
    startP.y = points[0].y;
    qsort(&points[1], n - 1, sizeof(CGPoint), compare);

    //Haben 2 oder mehr Punkte den gleichen Winkel mit startP,
    //werden alle Punkte entfernt, bis auf der der am weitesten weg ist

    //Initialgroesse des Arrays
    GLint m = 1;
    for (int i = 1; i < n; i++) {
        while ((i < (n - 1)) && (orientation(startP, points[i], points[i + 1]) == 0)) {
            i++;
        }
        points[m].x = points[i].x;
        points[m].y = points[i].y;
        m++;
    }

    //Bei weniger als 3 Punkten ist das bilden einer konvexen Huelle nicht moeglich
    if (m < 3) {
        return NULL;
    }

    stack_t *st = stack_new();
    stack_push(st, &points[0]);
    stack_push(st, &points[1]);
    stack_push(st, &points[2]);

    GLint stackSize = 3;

    //Uebrigen Punkte verarbeiten
    for (int i = 3; i < m; i++) {
        //Solange den obersten Punkte entfernen, bis der Winkel den die 3 Punkte aufspannen
        //ein nicht Links-Kurve machen
        while (orientation(secondToTop(st), *stack_top(st), points[i]) != 2) {
            stack_pop(st);
            stackSize--;
        }
        stack_push(st, &points[i]);
        stackSize++;
    }

    CGPoint2f *result = malloc(sizeof(CGPoint2f) * stackSize);
    GLint k = 0;

    //Stack leeren
    while (!stack_empty(st)) {
        CGPoint pt = *stack_top(st);
        result[k][CCX] = pt.x;
        result[k][CCY] = pt.y;
        stack_pop(st);
        k++;
    }
    *amntConvexHullVerts = stackSize;

    stack_destroy(st);
    return result;
}