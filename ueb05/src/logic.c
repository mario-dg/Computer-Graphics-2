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
#include <stdio.h>
#include <float.h>
#include <GL/glut.h>
#include <pthread.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "sceneObjects.h"
#include "multiThreading.h"
#include "io.h"
#include "trumboreMoeller.h"

/**---------------------------------------------- GLOBAL VARIABLES ----------------------------------------------*/

/** Szene die dargestellt wird */
static scene g_scene;

/**----------------------------------------- LOCAL FUNCTION DECLARATION -----------------------------------------*/
/**
 * Rendert die Szene mit multiplen Threads
 * @param args Argument Struct, mit allen Infos ueber den bereich den der Thread rendern soll
 * @return NULL, keine Rueckgabe, da direkt in g_data geschrieben wird
 */
void *logic_renderImageMultiThreaded(void *args);

/**
 * Rendert die Szene und speichert die Farbe jedes Pixels
 * im g_data Array
 */
static void logic_renderImage(void);

/**
 * Erstellt einen normalisierten Strahl, abhaengig von einem Punkt auf der Projektionsebene
 * @param GLint vertikaler Index
 * @param GLint horizontaler Index
 * @return Primaerstrahl von der Kamera durch das angegebene Pixel
 */
static Ray logic_createPrimaryRay(GLint i, GLint j);

/**
 * Erstellt einen Transmissions-Strahl, der die Richtung des original Strahls beibehaelt
 * (Alle Objekte in der Szene haben die gleiche Materialdichte)
 * @param originRay Ursprungs-Strahl
 * @param hitPoint Schnittpunkt mit der Szene des Ursprungsstrahls
 * @return Transmissions-Strahl
 */
static Ray logic_createTransmissionRay(Ray originRay, Hit hitPoint);

/**
 * Erstellt einen reflektiven Strahl, der im Einfallswinkel vom Objekt wieder ausstrahlt
 * @param originRay Ursprungs-Strahl
 * @param hitPoint Schnittpunkt mit der Szene des Ursprungsstrahls
 * @return Reflektions-Strahl
 */
static Ray logic_createReflectionRay(Ray originRay, Hit hitPoint);

/**
 * Verfolgt einen Strahl und liefert die Frabe des Pixels
 * @param Ray - Primaerstrahl
 *            - rekursiver Reflektionsstrahl
 *            - rekursiver Refraktionsstrahl
 * @param Ray depth aktuelle Tiefe der Rekursion
 * @return resultierende Farbe
 */
static Color logic_trace(Ray *, GLint);

/**
 * Prueft, ob der Ray ein Objekt in der Szene trifft
 * @param Ray Strahl er ein Objekt treffen soll
 * @return wenn kein Objekt getroffen wurde (Default Werte)
 *         wenn ein Objekt getroffen wurde (Infos ueber nahesten Punkt) ->
 *         (Distanz zum getroffenen Punkt,
 *          Index des getroffenen Punktes,
 *          Position des getroffenen Punktes,
 *          Normal des getroffenen Punktes)
 */
static Hit logic_hit(Ray);

/**
 * Berechnet die Farbe (Phong) an dem getroffenen Punkt und schaut, ob dieser im Schatten liegt
 * @param Ray Strahl, der auf das Objekt getroffen ist
 * @param Hit getroffener Punkt
 * @return Farbe an dem getroffenen Punkt
 */
static Color logic_calcPhong(Ray, Hit);

/**
 * Prueft ob der uebergeben HitPoint im Schatten, des Punktlichtes an index i liegt
 * @param Hit Getroffener Punkt in der Szene
 * @param GLint index des Punktlichtes
 * @return im Schatten oder nicht
 */
static GLboolean logic_shadowTrace(Hit, GLint);

/**
 * Reserviert den Speicher, der fuer die Szene benoetigt wird
 */
static void logic_initObjectData(void);

/**
 * Reserviert den Speicher fuer den Framebuffer abhaengig von der Aufloesung
 */
static void logic_initFramebuffer(void);

/**
 * Prueft ob ein Strahl eine Kugel trifft
 * !!! Effiziente Implementierung wichtig !!!
 * @param Ray Strahl
 * @param sphere Kugel
 * @return wenn die Kugel nicht getroffen wurde (Default Werte)
 *         wenn die Kugel getroffen wurde (Infos ueber nahesten Punkt) ->
 *         (Distanz zum getroffenen Punkt,
 *          Position des getroffenen Punktes)
 */
static Hit logic_raySphereIntersection(Ray, sphere);

/**
 * Adds a weighted color to another color
 * @param oldColor old Color, combined with the new Color
 * @param toAdd new Color
 * @param weight Weight of the new Color
 */
static void logic_addWeightedColor(Color *oldColor, Color toAdd, GLfloat weight);

/**
 * Erstellt ein Hit-Object mit allen benoetigten Informationen
 * @param dist Distanz vom getroffenen Punkt zum Start des Strahls
 * @param idxObject Index des getroffenen Objektes
 * @param position Position des getroffenen Punktes in der Szene
 * @param normal Normale der Position an der Szene
 * @return Hit-Object
 */
static Hit logic_copyHitPoint(GLfloat dist, objectModels idxObject, vec3 position, vec3 normal);

/**---------------------------------------- LOCAL FUNCTION IMPLEMENTATION ---------------------------------------*/

void *logic_renderImageMultiThreaded(void *args) {
    multiThreadRunner *runner = (multiThreadRunner *) args;
    //Ueber alle Pixel iterieren
    for (int j = runner->tileRow; j < runner->tileRow + runner->tileHeight; ++j) {
        for (int i = runner->tileCol; i < runner->tileCol + runner->tileWidth; ++i) {
            Ray ray = logic_createPrimaryRay(i, j);
            g_scene.fb[OFFSET2D(DEFAULT_WINDOW_HEIGHT, i, j)] = logic_trace(&ray, 1);
        }
    }
    return NULL;
}

static void logic_renderImage(void) {
    //Ueber alle Pixel iterieren
    for (int j = 0; j < yRes; ++j) {
        for (int i = 0; i < xRes; ++i) {
            Ray ray = logic_createPrimaryRay(i, j);
            g_scene.fb[OFFSET2D(DEFAULT_WINDOW_HEIGHT, i, j)] = logic_trace(&ray, 1);
        }
    }
}

static void logic_addWeightedColor(Color *oldColor, Color toAdd, GLfloat weight) {
    oldColor->r += toAdd.r * weight;
    oldColor->g += toAdd.g * weight;
    oldColor->b += toAdd.b * weight;
}

static Ray logic_createPrimaryRay(GLint i, GLint j) {
    Ray ray;
    //Projektionspunkt auf der Projektionsebene
    ray.start[0] = g_scene.projPlane.s[0] + ((float) i + 0.5f) * g_scene.projPlane.u[0] +
                   ((float) j + 0.5f) * g_scene.projPlane.v[0];

    ray.start[1] = g_scene.projPlane.s[1] + ((float) i + 0.5f) * g_scene.projPlane.u[1] +
                   ((float) j + 0.5f) * g_scene.projPlane.v[1];

    ray.start[2] = g_scene.projPlane.s[2] + ((float) i + 0.5f) * g_scene.projPlane.u[2] +
                   ((float) j + 0.5f) * g_scene.projPlane.v[2];

    //Startpunkt des Rays -> Kamera
    glm_vec3_sub(ray.start, g_scene.projPlane.cameraPos, ray.dir);
    ray.distance = 0.0f;
    glm_vec3_normalize(ray.dir);
    return ray;
}

static Ray logic_createTransmissionRay(Ray originRay, Hit hitPoint) {
    Ray transmissionRay;

    glm_vec3_copy(hitPoint.position, transmissionRay.start);
    glm_vec3_copy(originRay.dir, transmissionRay.dir);
    transmissionRay.distance = 0.0f;

    return transmissionRay;
}

static Ray logic_createReflectionRay(Ray originRay, Hit hitPoint) {
    Ray reflectionRay;

    glm_vec3_copy(hitPoint.position, reflectionRay.start);
    //Reflektionsrichtung
    utils_reflectDir(originRay.dir, hitPoint.normal, reflectionRay.dir);
    reflectionRay.distance = 0.0f;

    return reflectionRay;
}

static Color logic_trace(Ray *ray, GLint depth) {
    if (depth > RECURSION_DEPTH) {
        ray->distance += 0.0f;
        //Maximal Rekursionstiefe erreicht
        return BACKGROUND_COLOR;
    }

    //Sobald wir in rekursive Aufrufe gehen, sollen alle Waende geraendert werden
    if (depth > 1) {
        g_scene.projPlane.viewMode = ALL;
    }

    //Index vom dichtesten Objekt und das Dreieck was gerade getroffen wurde
    Hit hitPoint = logic_hit(*ray);
    if (hitPoint.defaultHit) {
        ray->distance += 0.0f;
        //Kein Objekt getroffen, Hintergrundfarbe zurueckgeben
        return BACKGROUND_COLOR;
    }

    //Material, vom gesetzten Objekt treffen
    sceneObjects_setHitObjectMaterial(g_scene, &hitPoint);

    //lokale Farbberechnung
    Color color = logic_calcPhong(*ray, hitPoint);

    //Insgesamte zurueckgelegte Strecke des Strahls anpassen
    ray->distance += glm_vec3_distance(hitPoint.position, ray->start);

    //Farbe abhaengig von der zurueckgelegten Distanz des Strahls abschwaechen
    utils_attenuationFunction(*ray, &color);

    //Rekursion abbrechen, wenn Intensitaet des Lichtes zu klein wird
    if (utils_colorIntensity(color) > MINIMUM_INTENSITIY) {
        //Objekte, die refraktieren
        if (hitPoint.material.kRefr > 0.0f) {
            //Transmissionstrahl
            Ray transmitRay = logic_createTransmissionRay(*ray, hitPoint);

            //Rekursiver Aufruf
            Color transmittedColor = logic_trace(&transmitRay, depth + 1);

            //Farbe aus dem rekursiven Aufruf gewichten
            logic_addWeightedColor(&color, transmittedColor, hitPoint.material.kRefr);

            //Insgesamte zurueckgelegte Strecke des Strahls anpassen
            ray->distance += transmitRay.distance;
        }

        //Objekte, die reflektieren
        if (hitPoint.material.kRefl > 0.0f) {
            //Reflektionsstrahl
            Ray reflectRay = logic_createReflectionRay(*ray, hitPoint);

            //Rekursiver Aufruf
            Color reflectedColor = logic_trace(&reflectRay, depth + 1);

            //Farbe aus dem rekursiven Aufruf gewichten
            logic_addWeightedColor(&color, reflectedColor, hitPoint.material.kRefl);

            //Insgesamte zurueckgelegte Strecke des Strahls anpassen
            ray->distance += reflectRay.distance;
        }
    }

    return color;
}

static Hit logic_copyHitPoint(GLfloat dist, objectModels idxObject, vec3 position, vec3 normal) {
    Hit result;

    result.defaultHit = GL_FALSE;
    result.dist = dist;
    result.idxObject = idxObject;
    glm_vec3_copy(position, result.position);
    glm_vec3_copy(normal, result.normal);

    return result;
}

static Hit logic_hit(Ray ray) {
    //Default Hit
    Hit result = utils_createDefaultHit();
    //Initial auf FLT_MAX fuers vergleichen setzen
    result.dist = FLT_MAX;

    //Bunny muss nicht geprueft werden, wenn die Bounding Box des
    //Bunnys nicht getroffen wurde
    GLboolean hitBB = GL_FALSE;

    //Ueber alle Objekte iterieren und SchnittPunkte berechnen
    for (int idxObj = 0; idxObj < AMOUNT_MODELS; ++idxObj) {
        if (idxObj == SPHERE) {
            //Pruefen, ob die Kugel getroffen wird
            //Andere Berechnung fuer die Intersection
            Hit temp = logic_raySphereIntersection(ray, g_scene.sphere);
            if (!temp.defaultHit) {
                //Normale der Kugel an dem Punkt bestimmen
                vec3 normal;
                glm_vec3_sub(temp.position, g_scene.sphere.center, normal);
                glm_vec3_normalize(normal);

                result = logic_copyHitPoint(temp.dist, SPHERE, temp.position, normal);
            }
        } else if (idxObj == BOUNDING_BOX) {
            if (g_scene.bbState != none) {
                //Pruefen ob die BoundingBox getroffen wird, wenn ja Schleife abbrechen
                for (int amountTris = 0;
                     amountTris < g_scene.allObjects[BOUNDING_BOX].faceCount && !hitBB; ++amountTris) {
                    //SchnittPunkt berechnen (Weber-Baldwin oder Trumbore-Moeller
                    Hit temp = trumboreMoeller_rayTriangleIntersection(ray,
                                                                       g_scene.allObjects[idxObj].facesTM[amountTris]);

                    if (!temp.defaultHit) {
                        //Ein und ausblenden der Bounding Box
                        if (g_scene.showBB) {
                            result = logic_copyHitPoint(temp.dist, BOUNDING_BOX,
                                                        temp.position,
                                                        g_scene.allObjects[idxObj].facesTM[amountTris].normal);
                        }
                        hitBB = GL_TRUE;
                    }
                }
            } else {
                //Wenn keine BoundingBox genutzt werden soll,
                //wird so getan, als wuerde sie getroffen werden, um dein Hasen normal abzufragen
                hitBB = GL_TRUE;
            }
        } else {
            //Ueber alle Dreiecke der Objekte iterieren, ausser Kugel
            for (int amountTris = 0; amountTris < g_scene.allObjects[idxObj].faceCount; ++amountTris) {
                //BoundingBox nicht getroffen, wenn der Hase gerendert werden soll
                //Hase muss garnicht erst getestet werden
                if (!hitBB && (idxObj == BUNNY) && (g_scene.bbState != none)) {
                    break;
                }

                //Die Wandseite von der wir aus schauen, soll im nicht rekursiven Durchgang nicht gerendert werden
                if (idxObj == g_scene.projPlane.viewMode) {
                    break;
                }

                //SchnittPunkt berechnen (Weber-Baldwin oder Trumbore-Moeller
                Hit temp = trumboreMoeller_rayTriangleIntersection(ray, g_scene.allObjects[idxObj].facesTM[amountTris]);

                //Wenn getroffen und berechnete Distanz kleiner als bisherige Distanz
                if (!temp.defaultHit && (temp.dist < result.dist)) {
                    result = logic_copyHitPoint(temp.dist, idxObj, temp.position,
                                                g_scene.allObjects[idxObj].facesTM[amountTris].normal);
                }
            }
        }
    }

    return result;
}

static Color logic_calcPhong(Ray ray, Hit hitPoint) {
    //Grundwert ist der Ambiente Anteil des Objektes
    Color col = {hitPoint.material.ka.r, hitPoint.material.ka.g, hitPoint.material.ka.b};
    //Wnn keine Lichtquelle aktiv ist, soll nichts zu sehen sein
    if (!g_scene.pointLights[0].active && !g_scene.pointLights[1].active) {
        col.r = 0.0f;
        col.g = 0.0f;
        col.b = 0.0f;
    }

    for (int i = 0; i < AMOUNT_LIGHTS; ++i) {
        //Nur wenn das Punktlicht aktiv ist beachten
        if (g_scene.pointLights[i].active) {
            //Liegt die Position im Schatten, keinen Farbwert berechnen
            //Spiegel soll keinen Schatten werden
            if (!logic_shadowTrace(hitPoint, i)) {
                //Richtungsvektor zum Licht, ausgehend vom getroffenen Punkt
                vec3 lightDir;
                glm_vec3_sub(g_scene.pointLights[i].pos, hitPoint.position, lightDir);
                glm_vec3_normalize(lightDir);

                /*-------------------- DIFFUSER ANTEIL --------------------*/
                GLfloat val = fmaxf(glm_vec3_dot(hitPoint.normal, lightDir), 0.0f);
                //Farbe ergibt sich, aus der diffusen Farbe des Objekts, des diffusen Anteils, der Lichtfarbe
                //und der Lichtintensitaet
                Color diffuse = {
                        hitPoint.material.kd.r * val * g_scene.pointLights[i].color.r *
                        g_scene.pointLights[i].intensity,
                        hitPoint.material.kd.g * val * g_scene.pointLights[i].color.g *
                        g_scene.pointLights[i].intensity,
                        hitPoint.material.kd.b * val * g_scene.pointLights[i].color.b *
                        g_scene.pointLights[i].intensity};

                /*-------------------- SPEKULARER ANTEIL --------------------*/
                vec3 negLightDir;
                glm_vec3_negate_to(lightDir, negLightDir);

                vec3 reflect;
                utils_reflectDir(negLightDir, hitPoint.normal, reflect);

                val = pow(fmaxf(glm_vec3_dot(ray.dir, reflect), 0.0f), hitPoint.material.shininess);

                //Farbe ergibt sich, aus der spekularen Farbe des Objekts und des spekularen Anteils
                Color specular = {hitPoint.material.ks.r * val,
                                  hitPoint.material.ks.g * val,
                                  hitPoint.material.ks.b * val};

                /*-------------------- ABSCHWAECHUNG DES PUNKTLICHTES --------------------*/
                //Abschwachung des Punktlichtes, abhangig von der Distanz zum getroffenen Punkt
                GLfloat distance = glm_vec3_distance(g_scene.pointLights[i].pos, hitPoint.position);
                GLfloat attenuation =
                        1.0 / (g_scene.pointLights[i].constant + g_scene.pointLights[i].linear * distance +
                               g_scene.pointLights[i].quadratic * (distance * distance));

                //Diffusen und Spekularen Anteil nochmal gewichten
                col.r += 0.8f * diffuse.r + 0.3f * specular.r;
                col.g += 0.8f * diffuse.g + 0.3f * specular.g;
                col.b += 0.8f * diffuse.b + 0.3f * specular.b;

                //Abschwaechen
                col.r *= attenuation;
                col.g *= attenuation;
                col.b *= attenuation;
            }
        }
    }
    return col;
}

static GLboolean logic_shadowTrace(Hit hitPoint, GLint i) {
    //Spiegel soll keinen Schatten werfen
    if (hitPoint.idxObject == MIRROR) {
        return GL_FALSE;
    }

    //normalisierter Schattenstrahl, vom Auftreffpunkt zur Lichtquelle an Index i
    Ray shadowRay;

    glm_vec3_copy(hitPoint.position, shadowRay.start);

    glm_vec3_sub(g_scene.pointLights[i].pos, shadowRay.start, shadowRay.dir);
    glm_vec3_normalize(shadowRay.dir);

    //Startposition des Schattenstrahls minimal entlang der Schattenstrahlrichtung verschieben,
    //um Float Ungenauigkeiten entgegenzuwirken, da der Strahl wieder am eigenen Objekt aufkommen koennte
    glm_vec3_add(shadowRay.start, (vec3) {shadowRay.dir[0] * BIAS, shadowRay.dir[1] * BIAS, shadowRay.dir[2] * BIAS},
                 shadowRay.start);

    //Abstand des getroffenen Punktes zum Licht
    GLfloat distToLight = glm_vec3_distance(g_scene.pointLights[i].pos, shadowRay.start);

    //Ueber alle Objekte iterieren und SchnittPunkte berechnen
    //Waende koennen nicht zwischen Lichtquelle und anderen Objekten in der Szene liegen
    //Kuerzeste Intersection wird nicht benoetigt
    for (int idxObj = CUBE; idxObj < AMOUNT_MODELS; ++idxObj) {
        //Kugel wieder extra abfragen
        if (idxObj == SPHERE) {
            Hit shadowHit = logic_raySphereIntersection(shadowRay, g_scene.sphere);
            GLboolean inShadow = !shadowHit.defaultHit;

            if (inShadow && (shadowHit.dist > EPSILON) && (shadowHit.dist < distToLight)) {
                return GL_TRUE;
            }
        } else {
            //Ueber alle Dreiecke der Objekte iterieren
            for (int amountTris = 0; amountTris < g_scene.allObjects[idxObj].faceCount; ++amountTris) {
                //SchnittPunkt berechnen (Weber-Baldwin oder Trumbore-Moeller
                Hit shadowHit = trumboreMoeller_rayTriangleIntersection(shadowRay,
                                                                        g_scene.allObjects[idxObj].facesTM[amountTris]);
                GLboolean inShadow = !shadowHit.defaultHit;

                //Schattennstrahl trifft ein Objekt, und ist dichter dran als die Lichtquelle
                if ((inShadow && (shadowHit.dist > EPSILON) && (shadowHit.dist < distToLight))) {

                    //Getroffenes Objekt ist eine Bounding Box, waehrend keine Bounding Box verwendet werden soll
                    //Schattenstrahl ignoriert die Bounding Box
                    if (idxObj == BOUNDING_BOX) {
                        //Ist nicht im Schatten
                        break;
                    }

                    //Ist im Schatten
                    return GL_TRUE;
                }
            }
        }
    }
    return GL_FALSE;
}

static void logic_initObjectData(void) {
    g_scene.allObjects = (object *) calloc(AMOUNT_MODELS, sizeof(struct object));
    if (g_scene.allObjects == NULL) {
        printf("Error initializing object Array!\n");
        exit(1);
    }
}

static void logic_initFramebuffer(void) {
    g_scene.fb = (Color *) calloc(DEFAULT_WINDOW_HEIGHT * DEFAULT_WINDOW_WIDTH, sizeof(*g_scene.fb));
    if (g_scene.fb == NULL) {
        printf("Error initializing data Array!\n");
        exit(1);
    }
}

static Hit logic_raySphereIntersection(Ray ray, sphere sp) {
    Hit result = utils_createDefaultHit();

    vec3 rayStartToSphereCenter;
    glm_vec3_sub(ray.start, sp.center, rayStartToSphereCenter);

    GLfloat b = glm_vec3_dot(rayStartToSphereCenter, ray.dir);

    GLfloat c = glm_vec3_norm2(rayStartToSphereCenter) - sp.radius * sp.radius;

    //Start des Rays ausserhalb der Kugel (c > 0) und zeigt weg von der Kugel (b > 0)
    if ((fabsf(c) < EPSILON) && (fabsf(b) < EPSILON)) return result;

    GLfloat discriminant = b * b - c;

    //negativer Diskriminant -> Strahl verfehlt Kugel
    if (discriminant < 0.0f) return result;

    //Strahl trifft Kugel, kuerzesten Abstand bestimmen
    result.dist = -b - sqrt(discriminant);

    //Getroffenen Punkt bestimmen
    result.position[0] = ray.start[0] + result.dist * ray.dir[0];
    result.position[1] = ray.start[1] + result.dist * ray.dir[1];
    result.position[2] = ray.start[2] + result.dist * ray.dir[2];
    result.defaultHit = GL_FALSE;

    return result;
}

/**--------------------------------------- GLOBAL FUNCTION IMPLEMENTATION ---------------------------------------*/

static void logic_render(void) {
    printf("Started Render!\n");
    if (g_scene.multiThreadOpts.useMultiThreading) {
        GLint amountThreads = g_scene.multiThreadOpts.verticalThreads *
                              g_scene.multiThreadOpts.horizontalThreads;

        //IDs der Threads erstellen
        g_scene.multiThreadOpts.threadIds = (pthread_t *) calloc(amountThreads, sizeof(pthread_t));
        for (int i = 0; i < amountThreads; ++i) {
            //Multithreading laufen lassen
            pthread_create(&g_scene.multiThreadOpts.threadIds[i], NULL,
                           logic_renderImageMultiThreaded, &g_scene.multiThreadOpts.threadArgs[i]);
        }

        /* Seit dem Programmstart vergangene Zeit in Millisekunden */
        int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

        for (int i = 0; i < amountThreads; ++i) {
            //Warten bis alle Threads fertig sind
            pthread_join(g_scene.multiThreadOpts.threadIds[i], NULL);

        }

        int lastCallTime = glutGet(GLUT_ELAPSED_TIME);
        /* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
        g_scene.renderTime = (double) (lastCallTime - thisCallTime) / 1000.0f;
    } else {
        /* Seit dem Programmstart vergangene Zeit in Millisekunden */
        int thisCallTime = glutGet(GLUT_ELAPSED_TIME);
        //Szene ohne Multithreading rendern
        logic_renderImage();
        int lastCallTime = glutGet(GLUT_ELAPSED_TIME);
        /* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
        g_scene.renderTime = (double) (lastCallTime - thisCallTime) / 1000.0f;
    }

    printf("Thread Amount: \t%d\n", g_scene.multiThreadOpts.threadingOpts);
    printf("Rendertime: \t%.3f Sekunden\n\n", g_scene.renderTime);
}

void logic_initLogic(void) {
    if (io_startRender()) {
        //Farbarray abhaengig von der Aufloesung initialisieren
        logic_initFramebuffer();

        //ViewPort Dimensionen festlegen (Quadratisch
        g_scene.projPlane.viewPortHeight = 2.0f;
        g_scene.projPlane.viewPortWidth = g_scene.projPlane.viewPortHeight;

        //Projektionsebene aufstellen
        sceneObjects_setViewDir(&g_scene, FRONT);

        //Standard BoundigBox Status fuer den Hasen setzen
        g_scene.bbState = aabb;
        g_scene.lastUsedBB = g_scene.bbState;
        //Bounding Box anzeigen
        g_scene.showBB = GL_TRUE;

        //MultiThreading Einstellungen festlegen
        multiThreading_setupThreading(&g_scene);

        //Objektarray initialisieren
        logic_initObjectData();
        //Modelle der Szene laden
        sceneObjects_initModels(&g_scene);
        //Punktlichter initialisieren
        sceneObjects_initPointLights(&g_scene);

        //Szene rendern
        logic_render();
    }
}

/**
 * Rendert die Szene erneut
 */
void logic_reDrawFrame(void) {
    if (g_scene.fb != NULL)
        free(g_scene.fb);
    logic_initFramebuffer();
    logic_render();
}

/**
 * Updated die Blickrichtung auf die Szene und rendert die Szene neu
 * @param mode
 */
void logic_updateViewDir(viewMode mode) {
    sceneObjects_setViewDir(&g_scene, mode);
    //Von hinten soll der Spiegel nicht gerendert werden
    if (g_scene.allObjects != NULL)
        free(g_scene.allObjects);
    logic_initObjectData();
    sceneObjects_initModels(&g_scene);
    logic_reDrawFrame();
}

/**
 * Gibt den reservierten Speicher wieder frei
 */
void logic_freeData(void) {
    if (g_scene.fb != NULL) {
        free(g_scene.fb);
    }
    if (g_scene.allObjects != NULL) {
        free(g_scene.allObjects);
    }
    if (g_scene.pointLights != NULL) {
        free(g_scene.pointLights);
    }
}

Color *logic_getFramebuffer(void) {
    return g_scene.fb;
}

void logic_togglePointLight1(void) {
    g_scene.pointLights[0].active = !g_scene.pointLights[0].active;
    if (g_scene.pointLights[1].active)
        printf("Enabled PointLight 1\n");
    else
        printf("Disabled PointLight 1\n");
    logic_reDrawFrame();
}

void logic_togglePointLight2(void) {
    g_scene.pointLights[1].active = !g_scene.pointLights[1].active;
    if (g_scene.pointLights[1].active)
        printf("Enabled PointLight 2\n");
    else
        printf("Disabled PointLight 2\n");
    logic_reDrawFrame();
}

void logic_toggleBoundingBoxes(void) {
    g_scene.bbState++;
    if (g_scene.bbState > none) {
        g_scene.bbState = 0;
        g_scene.allObjects[BOUNDING_BOX] = sceneObjects_initDefaultModel();
    }
    g_scene.allObjects[BOUNDING_BOX] = g_scene.bbState == aabb ? g_scene.boundingBoxes[0] : g_scene.boundingBoxes[1];

    switch (g_scene.bbState) {
        case none:
            printf("Switched to no Bounding Box for the Bunny\n");
            break;
        case aabb:
            printf("Switched to AABB for the Bunny\n");
            g_scene.lastUsedBB = aabb;
            break;
        case oobb:
            printf("Switched to OOBB for the Bunny\n");
            g_scene.lastUsedBB = oobb;
            break;
    }
    logic_reDrawFrame();
}

void logic_toggleShowBB(void) {
    g_scene.showBB = !g_scene.showBB;
    if (g_scene.showBB)
        printf("Displaying the Bounding Box for the Bunny\n");
    else
        printf("Not Displaying the Bounding Box for the Bunny\n");
    logic_reDrawFrame();
}

void logic_setThreadingOptions(multiThreadOptions opt) {
    g_scene.multiThreadOpts.threadingOpts = opt;
}
