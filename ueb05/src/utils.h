#ifndef RAYTRACER_UTILS_H
#define RAYTRACER_UTILS_H
#include "types.h"


/**
 * Erstellt ein Default Hit Objekt
 * @return
 */
Hit utils_createDefaultHit(void);

/**
 * Transformiert einen Vertex
 * @param vertex zu transformierenden Vertex
 * @param translation Ort
 * @param rotation Rotation
 * @param scale Skalierung
 */
void utils_transformVertices(vec3 vertex, vec3 translation, vec3 rotation, GLfloat scale);

/**
 * Berechnet die normalisierte Normale eines Dreiecks
 * Kreuzprodukt zweier Kanten des Dreiecks
 * @param tri Aktuelles Dreieck
 */
void utils_calcNormal(triangleTM *tri);

/**
 * Berechnet zwei Kanten des Dreiecks
 * @param tri Aktuelles Dreieck
 */
void utils_calcTwoEdgesTM(triangleTM *tri);

/**
 * Berechnet den Reflektionsstrahl anhand des urspruenglichen Strahls und der Normalen
 * @param ray urspruenglicher Strahl
 * @param normal Normal an dem Punkt
 * @param result Ergebnis
 */
void utils_reflectDir(vec3 ray, vec3 normal, vec3 result);

/**
 * Abschwaechungsfunktion der Farbe abhaengig von der Distanz des
 * getroffenen Punktes und der Kamera
 * @param Ray Strahl der diese Farbe erzeugt hat
 * @param Color Farbe die abgeschwaecht werden soll
 */
void utils_attenuationFunction(Ray ray, Color *color) ;

/**
 * Bestimmt die Luminosity einer Farbe
 * @param color Farbe
 * @return intensitaet
 */
GLfloat utils_colorIntensity(Color color);

/**
 * Verknuepft zwei char*
 * @param s1 string
 * @param s2 string 2
 * @return
 */
char *utils_concatStrings(const char *s1, const char *s2);
#endif //RAYTRACER_UTILS_H
