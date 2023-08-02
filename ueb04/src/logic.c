/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Wesentliche Programmlogik ist die
 * Verwaltung und Aktualisierung der Position und Bewegung eines Rechtecks. Die
 * Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Christopher Ploog, Mario da Graca
 */

/* ---- Standard Header einbinden ---- */
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "types.h"
#include "euler_integration.h"

/*-- Konstanten fuer Partikel --*/
#define PARTICLE_CNT 10
#define MIN_PARTCNT 1
#define MAX_PARTCNT 30

#define GOAL_CNT 2
#define ANIMATION_TIME (3.0f)
#define MIN_ANIMATION_PAUSE_TIME (1.0f)
#define MAX_ANIMATION_PAUSE_TIME (3.0f)

#define RANDOM_IN_RANGE(max) ((((float) rand() / (float) RAND_MAX) * (max)) - ((max) / 2.0f))
#define RANDOM_BETWEEN(min, max)  ((min) + ((float) rand() / (float) RAND_MAX)*((max) - (min)))

/* ---- Konstanten ---- */


/** Ziel Kugeln */
static CGVector3f g_Goals[GOAL_CNT];

/** Partikel Array */
static particle *g_allParticles;

GLint g_particleCnt = PARTICLE_CNT;

/** Ausgewaeltes Partikel */
static GLint selectedPart = 0;

/** Status der der Lichtberechnung (an/aus) */
static GLboolean g_lightingState = GL_TRUE;

/** Status der ersten Lichtquelle (an/aus) */
static GLboolean g_light0State = 1;

/** Folgemodus fuer Parikel */
static followMode fMode = followGoals;

/** Variablen fuer die zufaellige Bewegung der Ziele */
//Sollen sich die Ziele im Raum bewegen
static GLboolean g_moveGoals = GL_FALSE;
//Zeit die seit Beginn der Animation vergangen ist
static GLfloat g_timeSinceStartAnimation = 0.0f;
//Zeit die seit Ende der Animation vergangen ist
static GLfloat g_timeSinceStopAnimation = 0.0f;
//Laenge der Pause zwischen neuen Position
static GLfloat g_randomPauseTime = 0.0f;
//Boolean, ob sich die Ziele gerade in Bewegung befinden
static GLboolean g_isInAnimation = GL_FALSE;
//Neue Zielpositionen
static CGVector3f g_GoalNewPos[GOAL_CNT];


/**
 * Zufaelige Verteilung von Partikeln/Zielen
 * @param mode Modus, ob Ziele oder Partikel verteilt werden sollen
 * @param pos Position des zu verteilenden Objekts
 */
void randomePos(randMode mode, GLint pos) {
    //Partikel zufaellig verteilen
    if (mode == randParticles) {
        g_allParticles[pos].center[X] = RANDOM_IN_RANGE(AREA_WIDTH);
        g_allParticles[pos].center[Y] = RANDOM_IN_RANGE(AREA_WIDTH);
        g_allParticles[pos].center[Z] = RANDOM_IN_RANGE(AREA_WIDTH);

    } else {
        //Ziele zufaellig positionieren
        g_Goals[pos][X] = RANDOM_IN_RANGE(AREA_WIDTH);
        g_Goals[pos][Y] = RANDOM_IN_RANGE(AREA_WIDTH);
        g_Goals[pos][Z] = RANDOM_IN_RANGE(AREA_WIDTH);
    }
}

void initParticles(void) {
    //zufaellige ZielPos
    randomePos(randGoals, 0);
    randomePos(randGoals, 1);

    g_allParticles = malloc(g_particleCnt * sizeof(*g_allParticles));

    //Partikel zufaellig verteilen
    for (GLint i = 0; i < PARTICLE_CNT; i++) {
        randomePos(randParticles, i);
        g_allParticles[i].kWeak = RANDOM_BETWEEN(0.5f, 10.0f);
        g_allParticles[i].kVel = RANDOM_BETWEEN(1.0f, 2.0f);
    }
}

void getCenterPos(CGVector3f result) {
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    for (int i = 0; i < g_particleCnt; ++i) {
        result[0] += g_allParticles[i].center[X] * (1.0f / g_particleCnt);
        result[1] += g_allParticles[i].center[Y] * (1.0f / g_particleCnt);
        result[2] += g_allParticles[i].center[Z] * (1.0f / g_particleCnt);
    }

}

void calcParticleMove(GLfloat interval) {
    switch (fMode) {
        case followGoals:
            //Partikel folgen allen Zielen
            for (int i = 0; i < g_particleCnt; ++i) {
                calcEuler(&g_allParticles[i], g_Goals, GOAL_CNT, interval, 1.0f);
            }
            break;
        case followSingle:
            //Partikel folgen einzelnem Partikel
            for (int i = 0; i < g_particleCnt; ++i) {
                if (i == selectedPart) {
                    //Ausgewaehltes Partikel verfolgt weiterhin alle Ziele
                    calcEuler(&g_allParticles[i], g_Goals, GOAL_CNT, interval, 1.2f);
                } else {
                    calcEuler(&g_allParticles[i], &g_allParticles[selectedPart].center, 1, interval,
                              1.0f);
                }
            }
            break;
        case followAll: {
            //Partikel folgen Mittelpunkt aller Partikel
            CGVector3f centerPos;
            //Partikel-Mittelpunkt berechnen
            getCenterPos(centerPos);
            for (int i = 0; i < g_particleCnt; ++i) {
                calcEuler(&g_allParticles[i], &centerPos, 1, interval, 1.0f);
            }
        }
            break;
    }
}

/**
 * Aendert die Menge der Partikel und initialisiert diese
 * @param change -1/1 verkleiner oder vergroessern
 */
void updateParticles(GLint change) {
    //Pruefen ob neue Partikel-Zahl im Validen Berreich ist
    if (!((g_particleCnt + change > MAX_PARTCNT) || (g_particleCnt + change < MIN_PARTCNT))) {
        g_particleCnt += change;
        g_allParticles = realloc(g_allParticles, g_particleCnt * sizeof(*g_allParticles));

        //neues Partikel initialisieren
        if (change > 0) {
            GLint newPos = g_particleCnt - 1;
            randomePos(randParticles, newPos);
            g_allParticles[newPos].kWeak = RANDOM_BETWEEN(0.5f, 10.0f);
            g_allParticles[newPos].kVel = RANDOM_BETWEEN(1.0f, 2.0f);
        } else {
            //Ausgewaehltes Partikel anpassen
            if (selectedPart == g_particleCnt) {
                selectedPart = g_particleCnt - 1;
            }
        }
    }
}

/**
 * Interpoliert zeitabhaengig zwischen den alten und neuen Positionen der Ziele
 *
 * @param interval Interval
 */
void interpolateToNewCoords(GLfloat interval) {
    //Ueber alle Ziele iterieren
    for (int i = 0; i < GOAL_CNT; ++i) {
        //Absoluten Abstand zwischen alter Position und neuer Position
        GLfloat diffs[3] = {fabsf(g_GoalNewPos[i][0] - g_Goals[i][0]),
                            fabsf(g_GoalNewPos[i][1] - g_Goals[i][1]),
                            fabsf(g_GoalNewPos[i][2] - g_Goals[i][2])};
        for (int k = 0; k < 3; ++k) {
            //Wenn altes Ziel niedrigere Werte fuer die Koordinaten hat,
            //muss in positive Richtung bewegt werden, ansosnten in negative
            if (g_Goals[i][k] < g_GoalNewPos[i][k]) {
                g_Goals[i][k] += (diffs[k] * interval) * (1.0f / ANIMATION_TIME);
            } else {
                g_Goals[i][k] -= (diffs[k] * interval) * (1.0f / ANIMATION_TIME);
            }
        }

    }
}

void moveGoals(GLfloat interval) {
    //Nur, wenn Ziele sich bewegen sollen
    if (g_moveGoals) {
        //Wenn Ziele gerade in Bewegung sind
        if (g_isInAnimation) {
            g_timeSinceStartAnimation += interval;
            interpolateToNewCoords(interval);
            //Ziele haben sich in der vorgegebenen Animationszeit zur Zielposition bewegt
            if (g_timeSinceStartAnimation >= ANIMATION_TIME) {
                g_isInAnimation = GL_FALSE;
                g_timeSinceStartAnimation = 0.0f;
                //Zufaellige Pausenzeit zur naechsten Zielposition
                g_randomPauseTime = RANDOM_BETWEEN(MIN_ANIMATION_PAUSE_TIME, MAX_ANIMATION_PAUSE_TIME);
                for (int i = 0; i < GOAL_CNT; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        if (g_Goals[i][j] > 1.0f || g_Goals[i][j] < -1.0f) {
                            printf("ERROR\n");
                        }
                    }
                }
            }
        } else {
            //Ziele ruhen an Zielposition
            g_timeSinceStopAnimation += interval;
            //Pausenzeit ist vorbei
            if (g_timeSinceStopAnimation >= g_randomPauseTime) {
                g_isInAnimation = GL_TRUE;
                g_timeSinceStopAnimation = 0.0f;
                //Neue Zielposition zufaellig generieren
                for (int i = 0; i < GOAL_CNT; ++i) {
                    g_GoalNewPos[i][0] = RANDOM_IN_RANGE(AREA_WIDTH);
                    g_GoalNewPos[i][1] = RANDOM_IN_RANGE(AREA_WIDTH);
                    g_GoalNewPos[i][2] = RANDOM_IN_RANGE(AREA_WIDTH);
                }
            }
        }
    }
}

/**
 * Initialisiertt die Logik
 */
void initLogic(void) {

    selectedPart = 0;
    //Zufallsgenerator setzen
    srand(time(NULL));

    //Zufaellige Verteilung von Partikeln und Zielen
    initParticles();

}


/**
 * Liefert den Status der Lichtberechnung.
 * @return Status der Lichtberechnung (an/aus).
 */
int getLightingState(void) {
    return g_lightingState;
}

/**
 * Setzt den Status der Lichtberechnung.
 * @param status Status der Lichtberechnung (an/aus).
 */
void setLightingState(int status) {
    g_lightingState = status;
}

/**
 * Liefert den Status der ersten Lichtquelle.
 * @return Status der ersten Lichtquelle (an/aus).
 */
int getLight0State(void) {
    return g_light0State;
}

/**
 * Setzt den Status der ersten Lichtquelle.
 * @param status Status der ersten Lichtquelle (an/aus).
 */
void setLight0State(int status) {
    g_light0State = status;
}


void switchFollowMode(void) {
    switch (fMode) {
        case followGoals:
            fMode = followSingle;
            break;
        case followSingle:
            fMode = followAll;
            break;
        case followAll:
            fMode = followGoals;
            break;
    }
}

/**
 * Aendert das ausgewaelte Partikel
 */
void switchTargetPart(void) {
    selectedPart = (selectedPart + 1) % g_particleCnt;
}


GLint getParticleCount(void) {
    return g_particleCnt;
}

GLint getGoalCount(void) {
    return GOAL_CNT;
}

GLint getSelectedParticle(void) {
    return selectedPart;
}

particle *getAllParticles(void) {
    return g_allParticles;
}

followMode getFollowMode(void) {
    return fMode;
}

CGVector3f *getAllGoals(void) {
    return g_Goals;
}

void toggleMoveGoals(void) {
    g_moveGoals = !g_moveGoals;
}



