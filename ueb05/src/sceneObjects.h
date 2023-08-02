#ifndef RAYTRACER_SCENEOBJECTS_H
#define RAYTRACER_SCENEOBJECTS_H
#include "types.h"

/**
 * Erstellt ein Default Objekt der Szene
 * @return Default Objekt
 */
object sceneObjects_initDefaultModel();

void sceneObjects_loadBox(scene scene);

/**
 * Laedt die Objekte und platziert sie am richtigen Ort in der Szene
 */
void sceneObjects_initModels(scene *scene);

/**
 * Baut die Projektionsebene abhaengig von der Blickrichtung auf die Szene aus
 * @param mode Blickrichtung
 */
void sceneObjects_setViewDir(scene * scene, viewMode mode);

/**
 * Initialisiert die Punktlichter der Szene
 */
void sceneObjects_initPointLights(scene *scene);

/**
 * Setzt die Materialeigenschaften eines getroffenen Hit Objektes,
 * abhaengig von dem getroffenen Objekt in der Szene
 * @param hitObject Hit Objekt
 */
void sceneObjects_setHitObjectMaterial(scene scene, Hit *hitObject);
#endif //RAYTRACER_SCENEOBJECTS_H
