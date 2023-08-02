/**
 * @file
 * Ein-/Ausgabe-Modul.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.
 *
 * @author Mario Da Graca, Christopher Ploog
 */

/* ---- System Header einbinden ---- */
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "scene.h"
#include "io.h"

/* ---- Konstanten ---- */
#define TIMER_CALLS_PS 60 //Bilder pro Sekunde

GLint closestIndex = -1;

/**
 * Umschalten zw. Vollbild- und Fenstermodus.
 * Beim Wechsel zum Fenstermodus wird vorherige Position und Groesse
 * wiederhergestellt. HINWEIS: Fenster wird nicht korrekt auf alte Position
 * gesetzt, da GLUT_WINDOW_WIDTH/HEIGHT verfaelschte Werte liefert.
 */
static void
toggleFullscreen(void) {
    /* Flag: Fullscreen: ja/nein */
    static GLboolean fullscreen = GL_FALSE;
    /* Zwischenspeicher: Fensterposition */
    static CGPoint2i windowPos;
    /* Zwischenspeicher: Fenstergroesse */
    static CGDimensions2i windowSize;

    /* Modus wechseln */
    fullscreen = !fullscreen;

    if (fullscreen) {
        /* Fenstereinstellungen speichern */
        windowPos[0] = glutGet(GLUT_WINDOW_X);
        windowPos[1] = glutGet(GLUT_WINDOW_Y);
        windowSize[0] = glutGet(GLUT_WINDOW_WIDTH);
        windowSize[1] = glutGet(GLUT_WINDOW_HEIGHT);
        /* In den Fullscreen-Modus wechseln */
        glutFullScreen();
    } else {
        /* alte Fenstereinstellungen wiederherstellen */
        glutReshapeWindow(windowSize[0], windowSize[1]);

        /* HINWEIS:
           Auskommentiert, da es sonst Probleme mit der Vollbildarstellung bei
           Verwendung von FreeGlut gibt */
        glutPositionWindow(windowPos[0], windowPos[1]);
    }
}

/**
 * Setzen der Projektionsmatrix.
 * Setzt die Projektionsmatrix unter Berücksichtigung des Seitenverhaeltnisses
 * des Anzeigefensters, sodass das Seitenverhaeltnisse der Szene unveraendert
 * bleibt und gleichzeitig entweder in x- oder y-Richtung der Bereich von -1
 * bis +1 zu sehen ist.
 * @param aspect Seitenverhaeltnis des Anzeigefensters (In).
 */
static void
setProjection(GLdouble aspect) {
    /* Nachfolgende Operationen beeinflussen Projektionsmatrix */
    glMatrixMode(GL_PROJECTION);
    /* Matrix zuruecksetzen - Einheitsmatrix laden */
    glLoadIdentity();

    /* Koordinatensystem bleibt quadratisch */
    if (aspect <= 1) {
        gluOrtho2D(-1.0, 1.0,    /* links, rechts */
                   -1.0 / aspect, 1.0 / aspect); /* unten, oben */
    } else {
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect,  /* links, rechts */
                   -1.0, 1.0);   /* unten, oben */
    }
}

/**
 * Verarbeitung eines Tasturereignisses.
 * Pfeiltasten steuern die Position des angezeigten Rechtecks.
 * F1-Taste (de-)aktiviert Wireframemodus.
 * F2-Taste schaltet zwischen Fenster und Vollbilddarstellung um.
 * ESC-Taste und q, Q beenden das Programm.
 * Falls Debugging aktiviert ist, wird jedes Tastaturereignis auf stdout
 * ausgegeben.
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL>.
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen.
 * @param isSpecialKey ist die Taste eine Spezialtaste?
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 */
static void
handleKeyboardEvent(int key, int status) {
#define ESC 27

    if (status == GLUT_DOWN) {
        switch (key) {
            case GLUT_KEY_F1:
                toggleWireframeMode();
                break;
            case GLUT_KEY_F2:
                toggleFullscreen();
                break;
            case 'q':
            case 'Q':
            case ESC:
                freeAll();
                exit(0);
            case 'h':
            case 'H':
                toggleHelp();
                break;
            case 'c':
            case 'C':
                toggleConvex();
                break;
            case 'b':
            case 'B':
                toggleSplineBezier();
                if (!drawSplineBezier()) {
                    updateBezierScene(1);
                } else {
                    updateSplineScene(1);
                }
                break;
            case 'n':
            case 'N':
                toggleNormals();
                calculateNormals();
                break;
            case '+':
                if (adjustSteps(1)) {
                    updateSplineScene(1);
                }
                break;
            case '-':
                if (adjustSteps(-1)) {
                    updateSplineScene(1);
                }
                break;
            default:
                break;
        }
    }
}

/**
 * Bewegt einen Ausgewaehlten Vertex
 * @param x X-Pos des Mauszeigers
 * @param y Y-Pos des Mauszeigers
 */
void movement(GLint x, GLint y) {
    if (closestIndex > 0 && closestIndex < (getAmountVertices() - 1)) {
        moveVertex(x, y, (float) glutGet(GLUT_WINDOW_WIDTH), (float) glutGet(GLUT_WINDOW_HEIGHT), closestIndex);
        updateSplineScene(1);
    }
}

/**
 * Verarbeitung eines Mausevents
 * @param state Status der Taste
 * @param x X-Pos des Mauszeigers
 * @param y Y-Pos des Mauszeigers
 * @param button benutzte Maustaste
 */
void handleMouseEvent(GLint state, GLint x, GLint y, GLint button) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLfloat width = (float) glutGet(GLUT_WINDOW_WIDTH);
        GLfloat height = (float) glutGet(GLUT_WINDOW_HEIGHT);
        GLfloat aspect = width / height;
        CGPoint2f mousePos;
        if (aspect <= 1) {
            mousePos[CCX] = (x - (width / 2.0f)) / (width / 2.0f);
            mousePos[CCY] = -1.0f * (y - (height / 2.0f)) / ((height / 2) * aspect);
        } else {
            mousePos[CCX] = (x - (width / 2.0f)) / (width / 2.0f) * aspect;
            mousePos[CCY] = -1.0f * (y - (height / 2.0f)) / (height / 2);

        }
        closestIndex = getClosestIndex(mousePos);
        glutMotionFunc(movement);
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
cbKeyboard(int key, int x, int y) {
    handleKeyboardEvent(key, GLUT_DOWN);
}

/**
 * Callback fuer Maus-Ereignis.
 * Ruft Ereignisbehandlung fuer Maus-Ereignis auf.
 * @param button
 * @param state
 * @param x
 * @param y
 */
static void
cbMouse(int button, int state, int x, int y) {
    handleMouseEvent(state, x, y, button);
}

/**
 * Callback fuer Tastenloslassen.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Loslassens (In).
 * @param y y-Position der Maus zur Zeit des Loslassens (In).
 */
static void
cbKeyboardUp(int key, int x, int y) {
    handleKeyboardEvent(key, GLUT_UP);
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */

static void
cbKeyboardNorm(unsigned char key, int x, int y) {
    handleKeyboardEvent(key, GLUT_DOWN);
}

/**
 * Callback fuer Tastenloslassen.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Loslassens (In).
 * @param y y-Position der Maus zur Zeit des Loslassens (In).
 */
static void
cbKeyboardNormUp(unsigned char key, int x, int y) {
    handleKeyboardEvent(key, GLUT_UP);
}

/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer(int lastCallTime) {
    /* Seit dem Programmstart vergangene Zeit in Millisekunden */
    int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

    /* Wieder als Timer-Funktion registrieren */
    glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

    /* Neuzeichnen anstossen */
    glutPostRedisplay();
}

/**
 * Callback fuer Aenderungen der Fenstergroesse.
 * Initiiert Anpassung der Projektionsmatrix an veränderte Fenstergroesse.
 * @param w Fensterbreite (In).
 * @param h Fensterhoehe (In).
 */
static void
cbReshape(int w, int h) {
    /* Das ganze Fenster ist GL-Anzeigebereich */
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    /* Anpassen der Projektionsmatrix an das Seitenverhältnis des Fensters */
    setProjection((GLdouble) w / (GLdouble) h);
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void
cbDisplay(void) {
    /* Buffer zuruecksetzen */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Nachfolgende Operationen beeinflussen Modelviewmatrix */
    glMatrixMode(GL_MODELVIEW);

    /* Matrix zuruecksetzen - Einheitsmatrix laden */
    glLoadIdentity();

    /* Szene zeichnen */
    drawScene();

    /* Objekt anzeigen */
    glutSwapBuffers();   /* fuer DoubleBuffering */
}


/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void
registerCallbacks(void) {
    /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Spezielle-Taste gedrueckt wird */
    glutSpecialFunc(cbKeyboard);

    /* Tasten-Loslass-Callback - wird ausgefuehrt, wenn eine Spezielle-Taste losgelassen
     * wird */
    glutSpecialUpFunc(cbKeyboardUp);

    glutMouseFunc(cbMouse);

    /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
    glutKeyboardFunc(cbKeyboardNorm);

    /* Tasten-Loslass-Callback - wird ausgefuehrt, wenn eine Taste losgelassen
     * wird */
    glutKeyboardUpFunc(cbKeyboardNormUp);
    /* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
    glutTimerFunc(1000 / TIMER_CALLS_PS, /* msecs - bis Aufruf von func */
                  cbTimer,       /* func  - wird aufgerufen    */
                  glutGet(GLUT_ELAPSED_TIME));  /* value - Parameter, mit dem
                                                   func aufgerufen wird */

    /* Reshape-Callback - wird ausgefuehrt, wenn neu gezeichnet wird (z.B. nach
     * Erzeugen oder Groessenaenderungen des Fensters) */
    glutReshapeFunc(cbReshape);

    /* Display-Callback - wird an mehreren Stellen imlizit (z.B. im Anschluss an
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
int
initAndStartIO(char *title, int width, int height) {
    int windowID = 0;

    /* Kommandozeile immitieren */
    int argc = 1;
    char *argv = "cmd";

    /* Glut initialisieren */
    glutInit(&argc, &argv);

    /* Initialisieren des Fensters mit DoubleBuffering*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);


    /* Fenster erzeugen */
    windowID = glutCreateWindow(title);

    if (windowID) {

        if (initScene()) {
            registerCallbacks();
            glutMainLoop();
        } else {
            glutDestroyWindow(windowID);
            windowID = 0;
        }
    }

    return windowID;
}
