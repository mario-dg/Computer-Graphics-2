/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * @authors Mario Da Graca, Christopher Ploog
 */

/* ---- System Header einbinden ---- */
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdarg.h>

//Event musste included werden


/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "logic.h"
#include "stdlib.h"

//Vertex-Array
vertex *g_Vertices;

//Indize-Arrays
GLuint *g_ControlIndices;
GLuint *g_SplineIndices;
GLuint *g_BezierIndices;
GLuint *g_ConvexHullIndices;

//Umschalten von Zeichenfunktionen
GLint drawConvex = 0;
GLint drawNormals = 0;
GLint g_DrawHelp = 0;

/**
 * Zeichnen eines Einheitskreises
 */
void
drawCircle() {
/* Quadric erzeugen */
    GLUquadricObj *qobj = gluNewQuadric();
    if (qobj != 0) {
        /* Gefuellt darstellen */
        gluQuadricDrawStyle(qobj, GLU_FILL);
        /* Scheibe zeichnen */
        gluDisk(qobj, 0, 0.5, 20, 1);

        /* Loeschen des Quadrics */
        gluDeleteQuadric(qobj);
    }
}

/**
 * Zeichnen von Formatiertem Text
 * @param x X-Koordiante
 * @param y y-Koordiante
 * @param format zu vormatierender Text
 * @param ... Argumente für Text
 */
void
drawString(GLfloat x, GLfloat y, char *format, ...) {
    va_list args;
    char buffer[255];
    char *s;
    va_start(args, format);
    vsnprintf(buffer, 255, format, args);
    va_end(args);
    glRasterPos2f(x, y);
    for (s = buffer; *s; s++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s);
    }
}


/**
 * Zeichnen des Hilfemenues
 */
void
drawHelp() {
    glColor3f(0, 1, 0);
    drawString(-0.15, 0.8, "CONTROLS:");


    glColor3f(1, 1, 1);
    drawString(-0.4, 0.7, "ESC, Q, q");
    drawString(-0.4, 0.6, "H, h");
    drawString(-0.4, 0.5, "B, b");
    drawString(-0.4, 0.4, "C, c");
    drawString(-0.4, 0.3, "F1");
    drawString(-0.4, 0.2, "F2");
    drawString(-0.4, 0.1, "N, n");


    drawString(0.2, 0.7, "Quit Game");
    drawString(0.2, 0.6, "Open Help");
    drawString(0.2, 0.5, "Toggle Bezier-Curve");
    drawString(0.2, 0.4, "Toggle Convex-Hull");
    drawString(0.2, 0.3, "Toggle Wireframe");
    drawString(0.2, 0.2, "Toggle Fullscreen");
    drawString(0.2, 0.1, "Togle Normals");
}


/**
 * Zeichen-Funktion.
 * Stellt die Szene dar. Und zeichnet alles im derzeitigen Modus Sichtbare.
 */
void
drawScene() {
    if (g_DrawHelp) {
        drawHelp();
    } else {

        glPushMatrix();
        {
            glColor3f(0, 0, 1);
            //Kontroll-Verts zeichnen
            for (int i = 0; i < getAmountVertices(); ++i) {
                glPushMatrix();
                {
                    glTranslatef(g_Vertices[i][CCX], g_Vertices[i][CCY], 0);
                    glScalef(CIRCLE_RADIUS_CONTROL, CIRCLE_RADIUS_CONTROL, 1);
                    drawCircle();
                }
                glPopMatrix();
            }
        }
        glPopMatrix();


        glPushMatrix();
        {
            glColor3f(0, 1, 0);
            //Linie zeichnen
            glVertexPointer(2,
                            GL_FLOAT,
                            sizeof(vertex),
                            &(g_Vertices[0][CX]));
            glDrawElements(GL_LINE_STRIP,
                           getAmountCurveVertices(),
                           GL_UNSIGNED_INT,
                           g_SplineIndices);
        }
        glPopMatrix();

        if (drawConvex) {
            //Konvexe-Huelle zeichnen
            glPushMatrix();
            {
                glColor3f(1, 0, 0);
                glVertexPointer(2,
                                GL_FLOAT,
                                sizeof(vertex),
                                &(g_Vertices[0][CHX]));


                glDrawElements(GL_LINE_STRIP,
                               getAmountConvexHullVertices() + 1,
                               GL_UNSIGNED_INT,
                               g_ConvexHullIndices);
            }
            glPopMatrix();
        }

        if (drawNormals) {
            //Normals zeichnen
            for (int i = 0; i < getAmountCurveVertices(); i++) {
                glPushMatrix();
                {
                    glColor3f(1, 0, 1);
                    glTranslatef(g_Vertices[i][CX], g_Vertices[i][CY], 0.0f);
                    glBegin(GL_LINES);
                    {
                        glVertex2f(0, 0);
                        glVertex2f(g_Vertices[i][CNX] / 10.0f,
                                   g_Vertices[i][CNY] / 10.0f);
                    }
                    glEnd();
                }
                glPopMatrix();
            }
        }
    }
}


/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void
toggleWireframeMode(void) {
    /* Flag: Wireframe: ja/nein */
    static GLboolean wireframe = GL_FALSE;

    /* Modus wechseln */
    wireframe = !wireframe;

    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

/**
 * Vertex-Array fuellen
 * @param update (0=Erstellen; 1=updaten)
 */
void fillVertexArray(GLint update) {
    CGPoint2f *controlVertices = getControlVertices();
    CGPoint2f *curveVertices = getCurveVertices();
    CGPoint2f *convexHullVertices = getConvexHullVertices();
    CGPoint2f *normals = getNormals();

    GLint curveVerts = getAmountCurveVertices();

    if (update) {
        g_Vertices = realloc(g_Vertices, sizeof(vertex) * curveVerts);
    } else {
        g_Vertices = malloc(sizeof(vertex) * curveVerts);
    }

    if (!g_Vertices) {
        exit(1);
    }

    //Kontroll-Verts in Vert Array uebertragen
    for (int i = 0; i < getAmountVertices(); i++) {
        g_Vertices[i][CCX] = controlVertices[i][CCX];
        g_Vertices[i][CCY] = controlVertices[i][CCY];
    }

    //Curve-und Normal-Verts in Vert Array uebertragen
    for (int i = 0; i < curveVerts; i++) {
        g_Vertices[i][CX] = curveVertices[i][CCX];
        g_Vertices[i][CY] = curveVertices[i][CCY];
        g_Vertices[i][CNX] = normals[i][CCX];
        g_Vertices[i][CNY] = normals[i][CCY];

    }
    //Konvex-Huellen-Verts in Vert Array uebertragen
    for (int i = 0; i < getAmountConvexHullVertices(); ++i) {
        g_Vertices[i][CHX] = convexHullVertices[i][CCX];
        g_Vertices[i][CHY] = convexHullVertices[i][CCY];

    }
}

/**
 * Kontroll-Index-Array befuellen
 */
void fillControlIndiceArray() {
    g_ControlIndices = malloc(sizeof(GLint) * getAmountVertices());
    if (!g_ControlIndices) {
        free(g_SplineIndices);
        exit(1);
    }

    for (int i = 0; i < getAmountVertices(); ++i) {
        g_ControlIndices[i] = i;
    }
}

/**
 * Spline-Index-Array befuellen / updaten
 * @param update (0=Erstellen; 1=updaten)
 */
void fillSplineIndiceArray(GLint update) {
    if (update) {
        g_SplineIndices = realloc(g_SplineIndices, sizeof(GLint) * getAmountCurveVertices());
    } else {
        g_SplineIndices = malloc(sizeof(GLint) * getAmountCurveVertices());
    }
    if (!g_SplineIndices) {
        exit(1);
    }

    for (int i = 0; i < getAmountCurveVertices(); ++i) {
        g_SplineIndices[i] = i;
    }
}

/**
 * Bezier-Index-Array fuellen
 * @param update (0=Erstellen; 1=updaten)
 */
void fillBezierIndiceArray(GLint update) {
    if (update) {
        g_BezierIndices = realloc(g_SplineIndices, sizeof(GLint) * getAmountCurveVertices());
    } else {
        g_BezierIndices = malloc(sizeof(GLint) * getAmountCurveVertices());
    }
    if (!g_BezierIndices) {
        exit(1);
    }

    for (int i = 0; i < getAmountCurveVertices(); ++i) {
        g_BezierIndices[i] = i;
    }
}

/**
 * Convex-Hull-Array fuellen
 * @param update (0=Erstellen; 1=updaten)
 */
void fillConvexHullIndiceArray(GLint update) {
    //Erster Punkt muss am Ende wieder indiziert werdem
    if (update) {
        g_ConvexHullIndices = realloc(g_ConvexHullIndices, sizeof(GLint) * getAmountConvexHullVertices() + 1);
    } else {
        g_ConvexHullIndices = malloc(sizeof(GLint) * getAmountConvexHullVertices() + 1);
    }

    if (!g_ConvexHullIndices) {
        free(g_SplineIndices);
        free(g_ControlIndices);
        exit(1);
    }

    for (int i = 0; i < getAmountConvexHullVertices(); ++i) {
        g_ConvexHullIndices[i] = i;
    }
    //Konvexe Huelle soll geschlossen gezeichnet werden
    g_ConvexHullIndices[getAmountConvexHullVertices()] = 0;
}

/**
 * Vert-Array initialisieren
 */
void initVertexArray() {
    fillVertexArray(0);
    fillSplineIndiceArray(0);
    fillControlIndiceArray();
    fillConvexHullIndiceArray(0);
}

/**
 * Initialisiert die Szene.
 */
int
initScene() {
    /* Hintergrundfarbe */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    /* Zeichenfarbe */
    glColor3f(1.0f, 1.0f, 1.0f);
    init();
    initVertexArray();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    return (1);
}

/**
 * Allen genutzeten Spiecherplatz freigen
 */
void freeAll(void) {
    free(g_Vertices);
    free(g_SplineIndices);
    free(g_ControlIndices);
}

/**
 * Spine updaten / erstellen
 * @param update (0=Erstellen; 1=updaten)
 */
void updateSplineScene(GLint update) {
    fillVertexArray(update);
    fillSplineIndiceArray(update);
    fillConvexHullIndiceArray(update);
}

/**
 * Bezier updaten
 */
void updateBezierScene() {
    fillVertexArray(1);
    fillBezierIndiceArray(1);
    fillConvexHullIndiceArray(1);
}

void toggleConvex() {
    drawConvex = !drawConvex;
}

void toggleNormals() {
    drawNormals = !drawNormals;
}

void toggleHelp() {
    g_DrawHelp = !g_DrawHelp;
}

