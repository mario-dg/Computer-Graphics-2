/**
 * @file
 * Ein-/Ausgabe-Modul.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.

 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else

#include <GL/glut.h>

#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "scene.h"
#include "debugGL.h"
#include "logic.h"

/* ---- Konstanten ---- */

static GLboolean g_startRender = GL_FALSE;

/* ---- Funktionen ---- */

GLboolean io_startRender(){
    return g_startRender;
}

static void io_printHelp(void){
    printf("\nHelp:\n");
    printf("F1 :       16 Threads Rendering\n");
    printf("F2 :       8  Threads Rendering\n");
    printf("F3 :       4  Threads Rendering\n");
    printf("F4 :       2  Threads Rendering\n");
    printf("F5 :       1  Thread  Rendering\n");
    printf("q/Q:          Exit the Program\n");
    printf("g/G:          Disable Pointlight 1\n");
    printf("f/F:          Disable Pointlight 2\n");
    printf("b/B:          (Un-)Show Bounding Box\n");
    printf("n/N:          Toggle between Bounding Boxes\n");
    printf("v/V:          View Scene from Front\n");
    printf("h/H:          View Scene from Behind\n");
    printf("o/O:          View Scene from Above\n");
    printf("u/U:          View Scene from Below\n");
    printf("l/L:          View Scene from the Left\n");
    printf("r/R:          View Scene from the Right\n\n");
}

/**
 * Verarbeitung eines Tasturereignisses.
 * ESC-Taste und q, Q beenden das Programm.
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL>.
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen.
 * @param isSpecialKey ist die Taste eine Spezialtaste?
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 */
static void
handleKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x, int y) {
    (void) x;
    (void) y;


    /** Keycode der ESC-Taste */
#define ESC 27

    /* Taste gedrueckt */
    if (status == GLUT_DOWN) {
        /* Spezialtaste gedrueckt */
        if (isSpecialKey) {
            multiThreadOptions opt = noMultiThreading;
            switch (key) {
                    /* Render mit 16 Threads */
                case GLUT_KEY_F1:
                    opt = threads16;
                    break;
                    /* Render mit 16 Threads */
                case GLUT_KEY_F2:
                    opt = threads8;
                    break;
                    /* Render mit 16 Threads */
                case GLUT_KEY_F3:
                    opt = threads4;
                    break;
                    /* Render mit 16 Threads */
                case GLUT_KEY_F4:
                    opt = threads2;
                    break;
                    /* Render mit 16 Threads */
                case GLUT_KEY_F5:
                    opt = noMultiThreading;
                    break;

            }
            logic_freeData();
            logic_setThreadingOptions(opt);
            g_startRender = GL_TRUE;
            logic_initLogic();
        } else {
            /* normale Taste gedrueckt */
            switch (key) {
                /* Programm beenden */
                case 'q':
                case 'Q':
                case ESC:
                    logic_freeData();
                    exit(0);
                case 'v':
                case 'V':
                    if(g_startRender)
                        logic_updateViewDir(FRONT);
                    break;
                case 'h':
                case 'H':
                    if(g_startRender)
                        logic_updateViewDir(BACK);
                    break;
                case 'l':
                case 'L':
                    if(g_startRender)
                        logic_updateViewDir(LEFT);
                    break;
                case 'r':
                case 'R':
                    if(g_startRender)
                        logic_updateViewDir(RIGHT);
                    break;
                case 'o':
                case 'O':
                    if(g_startRender)
                        logic_updateViewDir(TOP);
                    break;
                case 'u':
                case 'U':
                    if(g_startRender)
                        logic_updateViewDir(BOTTOM);
                    break;
                case 'n':
                case 'N':
                    if(g_startRender)
                        logic_toggleBoundingBoxes();
                    break;
                case 'b':
                case 'B':
                    if(g_startRender)
                        logic_toggleShowBB();
                    break;
                case 'g':
                case 'G':
                    if(g_startRender)
                        logic_togglePointLight1();
                    break;
                case 'f':
                case 'F':
                    if(g_startRender)
                        logic_togglePointLight2();
                    break;
                case 't':
                case 'T':
                    io_printHelp();
                    break;
            }
        }
        //Da die Szene sich geaendert hat, muss DrawScene()
        //erneut aufgerufen werden
        glutPostRedisplay();
    }
}


/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbKeyboard(unsigned char key, int x, int y) {
    handleKeyboardEvent(key, GLUT_DOWN, GL_FALSE, x, y);
}

/**
 * Callback fuer Druck auf Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbSpecial(int key, int x, int y) {
    handleKeyboardEvent(key, GLUT_DOWN, GL_TRUE, x, y);
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void
cbDisplay(void) {
    /* Framewbuffer und z-Buffer zuruecksetzen */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawScene();

    glutSwapBuffers(); /* fuer DoubleBuffering */
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void
registerCallbacks(void) {

    /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
    glutKeyboardFunc(cbKeyboard);

    /* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
     * (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
     * Einfuegen) gedrueckt wird */
    glutSpecialFunc(cbSpecial);

    /* Display-Callback - wird an mehreren Stellen implizit (z.B. im Anschluss an
     * Reshape-Callback) oder explizit (durch glutPostRedisplay) angestossen */
    glutDisplayFunc(cbDisplay);

}

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int initAndStartIO(char *title, int width, int height) {
    int windowID = 0;

    /* Kommandozeile immitieren */
    int argc = 1;
    char *argv = "cmd";

    /* Glut initialisieren */
    glutInit(&argc, &argv);

    /* DEBUG-Ausgabe */
    INFO(("Erzeuge Fenster...\n"));

    /* Initialisieren des Fensters */
    /* RGB-Framebuffer, Double-Buffering und z-Buffer anfordern */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);

    /* Fenster erzeugen */
    windowID = glutCreateWindow(title);

    if (windowID) {

        /* DEBUG-Ausgabe */
        INFO(("...fertig.\n\n"));

        /* DEBUG-Ausgabe */
        INFO(("Initialisiere Szene...\n"));
        //logic_initLogic();
        if (initScene()) {

            /* DEBUG-Ausgabe */
            INFO(("...fertig.\n\n"));

            /* DEBUG-Ausgabe */
            INFO(("Registriere Callbacks...\n"));

            registerCallbacks();

            /* DEBUG-Ausgabe */
            INFO(("...fertig.\n\n"));

            /* DEBUG-Ausgabe */
            INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));

            glutMainLoop();

            /* DEBUG-Ausgabe */
            INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));

            glutMainLoop();
        } else {
            /* DEBUG-Ausgabe */
            INFO(("...fehlgeschlagen.\n\n"));

            glutDestroyWindow(windowID);
            windowID = 0;
        }
    } else {
        /* DEBUG-Ausgabe */
        INFO(("...fehlgeschlagen.\n\n"));
    }

    return windowID;
}
