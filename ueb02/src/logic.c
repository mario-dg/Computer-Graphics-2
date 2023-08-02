
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
 * @author Mario da Graca, Christopher Ploog
 */

/* ---- Standard Header einbinden ---- */
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "scene.h"

/** Anzahl der Drehungen des rotierenden Lichtes pro Sekunde */
#define LIGHT1_ROTATIONS_PS 0.25f

#define PICK_HEIGHT (0.5f)

#define MAX_HEIGHT (0.5f)

#define RANDOM_HEIGHT (((float) rand() / (float) RAND_MAX) * MAX_HEIGHT);
/* ---- Konstanten ---- */

/** Status der der Lichtberechnung (an/aus) */
static GLboolean g_lightingState = GL_TRUE;

/** Status der ersten Lichtquelle (an/aus) */
static GLboolean g_light0State = 1;

/** Status der zweiten Lichtquelle (rotierend/aus/boot1/boot2) */
static light1State g_light1State = 0;

/** Rotationswinkel der zweiten Lichtquelle. */
static float g_light1RotationAngle = 0.0f;

/** Hoehen der Wassersaeulen in einem dynamisch alloziierten Array*/
static GLfloat *heights;

static GLfloat* splineHeights;

void freeAllocatedMemLogic(void)
{
    free(heights);
}

void initLogic(void)
{
    srand(time(NULL));
    GLint amountVerticesSide = START_AMOUNT_VERTICES;
    heights = calloc(SQUARE(amountVerticesSide), sizeof(GLfloat));
    if (heights == NULL)
    {
        exit(1);
    }
    //zufaellige Hoehe
    for (int i = 0; i < SQUARE(amountVerticesSide); ++i) {
        heights[i] = RANDOM_HEIGHT;
    }
}


void updateLogic(expandShrinkVertices state)
{
    GLint i = 0;
    GLint rowCount = -1;
    GLint oldAmountVerticesSide = getAmountVertices();
    GLint newAmountVerticesSide = oldAmountVerticesSide + state;

    //alte Hoehen speichern um diese beim Vergroeßern oder Verkleinern nicht zu verlieren
    GLfloat *oldHeights = calloc(SQUARE(oldAmountVerticesSide), sizeof(GLfloat));
    if (oldHeights == NULL)
    {
        exit(1);
    }


    for (i = 0; i < SQUARE(oldAmountVerticesSide); i++)
    {
        oldHeights[i] = heights[i];
    }

    //Speicher des Hoehenarrays realloziieren
    heights = realloc(heights, SQUARE(newAmountVerticesSide) * sizeof(GLfloat));
    if (heights == NULL)
    {
        exit(1);
    }

    //Fuellen des neuen Hoehen- und Geschwindigkeitsrrays
    for (i = 0; i < SQUARE(newAmountVerticesSide); i++)
    {
        if ((i % newAmountVerticesSide) == 0)
        {
            rowCount++;
        }

        if (state == expand)
        {
            //neue Punkte haben die Hoehe und Geschwindigkeit 0
            if (((i % newAmountVerticesSide) == (newAmountVerticesSide - 1)) || ((i / newAmountVerticesSide) == (newAmountVerticesSide - 1)))
            {
                heights[i] = RANDOM_HEIGHT;
            }
                //alte Punkte behalten ihre Werte
            else
            {
                heights[i] = oldHeights[i - rowCount];
            }
        }
            //Beim Verkleinern werden die Randpunkte einfach abgeschnitten
        else
        {
            heights[i] = oldHeights[i + rowCount];
        }
    }

    //Freigabe der Zwischenspeicher
    free(oldHeights);
    free(splineHeights);

}

void pickedVertex(GLuint index, int direction)
{
    heights[index] += direction * PICK_HEIGHT;
}

/**
 * Liefert den Status der Lichtberechnung.
 * @return Status der Lichtberechnung (an/aus).
 */
int getLightingState(void)
{
    return g_lightingState;
}

/**
 * Setzt den Status der Lichtberechnung.
 * @param status Status der Lichtberechnung (an/aus).
 */
void setLightingState(int status)
{
    g_lightingState = status;
}

/**
 * Liefert den Status der ersten Lichtquelle.
 * @return Status der ersten Lichtquelle (an/aus).
 */
int getLight0State(void)
{
    return g_light0State;
}

/**
 * Setzt den Status der ersten Lichtquelle.
 * @param status Status der ersten Lichtquelle (an/aus).
 */
void setLight0State(int status)
{
    g_light0State = status;
}

/**
 * Berechnet aktuellen Rotationswinkel der zweiten Lichtquelle.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void calcLight1Rotation(double interval)
{
    if ((g_light1State == rotating) && g_lightingState)
    {
        g_light1RotationAngle += 360.0f * LIGHT1_ROTATIONS_PS * (float)interval;
    }
}

/**
 * Liefert den aktuellen Rotationswinkel der zweiten Lichtquelle.
 * @return aktueller Rotationswinkel der zweiten Lichtquelle.
 */
float getLight1Rotation(void)
{
    return g_light1RotationAngle;
}

GLfloat *getHeights(void)
{
    return heights;
}

