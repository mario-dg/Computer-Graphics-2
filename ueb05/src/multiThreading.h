#ifndef RAYTRACER_MULTITHREADING_H
#define RAYTRACER_MULTITHREADING_H
#include "types.h"

/**
 * Erstellt einen Thread anhand der eingestellten Multithreading Optionen
 * @param scene aktuelle Szene
 */
void multiThreading_setupThreading(scene *scene);

#endif //RAYTRACER_MULTITHREADING_H
