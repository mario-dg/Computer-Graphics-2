/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Textausgabe mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else

#include <GL/gl.h>
#include "stringOutput.h"

#endif

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "debugGL.h"
#include "logic.h"
#include "io.h"

/**
* Gibt den Setuptext aus.
*/
static void drawSetup() {
    int size = 7;

    Color color = {95 / 255.0f, 212 / 255.0f, 207 / 255.0f};

    char *help[] = {"Multithreading Settings:",
                    "F1     -    16 Threads",
                    "F2     -    8 Threads",
                    "F3     -    4 Threads",
                    "F4     -    2 Threads",
                    "F5     -    no Multithreading",
                    "t/T    -    print Help"};

    drawString(0.35f, 0.1f, color, help[0]);

    for (GLint i = 1; i < size; ++i) {
        drawString(0.1f, 0.15f + i * 0.1f, color, help[i]);
    }
}

/**
 * Zeichen-Funktion, stellt die Szene dar
 */
void drawScene(void) {
    if(!io_startRender()){
        drawSetup();
    } else {
        //Szene einmal zeichnen, nachdem sie fertig gerendert wurde
        glDrawPixels(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, GL_RGB, GL_FLOAT, logic_getFramebuffer());
    }
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void) {
    /* Setzen der Farbattribute */
    /* Hintergrundfarbe */
    glClearColor(11 / 255.0f, 12 / 255.0f, 16 / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Alles in Ordnung? */
    return (GLGETERROR == GL_NO_ERROR);
}
