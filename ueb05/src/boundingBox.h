#ifndef RAYTRACER_BOUNDINGBOX_H
#define RAYTRACER_BOUNDINGBOX_H
#include "utils.h"

/**
 * Erstellt aus einer Bounding ein Objekt, welches in der Szene gerendert werden kann
 * @param bBox Bounding Box (AABB / OOBB)
 * @return Objekt fuer die Szene
 */
object boundingBox_createObjectFromBoundingBox(corners corner);

/**
 * Erstellt fuer ein Objekt aus der Szene eine AABB
 * @param currObj Objekt aus der Szene
 * @return AABB fuer dieses Objekt
 */
boundingBox boundingBox_calculateAABB(object currObj, corners* corner);

/**
 * Erstellt eine OOBB fuer ein Objekt aus der Szene
 * @param aabb AABB des Objektes
 * @param obj Objekt aus der Szene
 * @return OOBB fuer dieses Objekt
 */
void boundingBox_createOOBFromAABB(boundingBox aabb, object obj, corners *corner, vec3 bunnyTranslation);

#endif //RAYTRACER_BOUNDINGBOX_H