//
// Created by mario on 01.01.21.
//

#ifndef UEB05_LOADOBJ_H
#define UEB05_LOADOBJ_H
#include "utils.h"
/**
 * Laedt eine .Obj Datei und erstellt ein Objekt, welches in der Szene gerendert werden kann
 * Erstellt aus Vertizes und Indizes ein Objekt und platziert es in der Szene
 * @param fileName Dateiname der obj Datei
 * @param translation Ort des Objektes in der Szene
 * @param rotation Rotation des Objektes
 * @param scale Skalierung des Objektes
 * @return object, geladenenes object, default Object, wenn was schief gegangen ist
 */
object loadObj_readFile(const char* fileName, vec3 translation, vec3 rotation, GLfloat scale);
#endif //UEB05_LOADOBJ_H
