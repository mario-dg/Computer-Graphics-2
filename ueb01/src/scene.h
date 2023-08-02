/**
 * @file
 * Schnittstelle des Darstellungs-Moduls.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * @author Mario Da Graca, Christopher Ploog
 */


#ifndef UEB02_SCENE_H
#define UEB02_SCENE_H

#include "types.h"

int initScene();
void toggleWireframeMode();
void drawScene();
void freeAll(void);
void initVertexArray();
void updateSplineScene(GLint update);
void updateBezierScene();
void toggleConvex();
void toggleNormals();
void toggleHelp();

#endif //UEB02_SCENE_H
