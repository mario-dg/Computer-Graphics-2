#ifndef __RAYTRACER_TYPES_H__
#define __RAYTRACER_TYPES_H__
/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
 *
 *
 * @author copyright (C) Fachhochschule Wedel 1999-Z011. All rights reserved.
 * @author Christopher Ploog, Mario da Graca
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else

#include <GL/gl.h>

#endif

#include <cglm/cglm.h>

/** ------------------------------------------------- Konstanten ------------------------------------------------- */

/** Default Fenster Groesse */
#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 640
/** Aufloesung des Ergbenisses */
#define xRes (DEFAULT_WINDOW_WIDTH - 1)
#define yRes (DEFAULT_WINDOW_HEIGHT - 1)

/**Maximale Rekursionstiefe (Startet bei 1)*/
#define RECURSION_DEPTH (3)
/** Anzahl der Punktlichtquellen */
#define AMOUNT_LIGHTS (2)
/** Minimale Intensitaet die berechent werden muss, damit Rekursion fortgefuehrt wird */
#define MINIMUM_INTENSITIY (0.05f)

/**Objekt Konstanten*/
#define CUBE_SCALE (0.4f)
#define SPHERE_RADIUS (0.25f)
#define BUNNY_SCALE (0.3f)

/** Bias zum verschieben, des Ray Startes entlang seiner Richtung */
#define BIAS (0.0001f)

/** Epsilon fuer Float vergleiche */
#define EPSILON (0.000001f)

/** Wandelt 2D Koordinaten in 1D um */
#define OFFSET2D(WIDTH, x, y) ((x) + (WIDTH) * (y))

/** ---------------------------------------------- Typedeklarationen ---------------------------------------------- */

/** Zaehlt alle in der Szene verwendeten Objekte auf */
typedef enum objectModels {
    FRONT_WALL,
    REAR_WALL,
    UPPER_WALL,
    LOWER_WALL,
    LEFT_WALL,
    RIGHT_WALL,
    CUBE,
    MIRROR,
    SPHERE,
    BOUNDING_BOX,
    BUNNY,
    AMOUNT_MODELS
} objectModels;

/** Repraesentation eines Strahls mit einem Startpunkt und einer Richtung */
typedef struct Ray {
    vec3 start, dir;
    GLfloat distance;
} Ray;

/** Struct, zur Repraesentation einer RGB Farbe */
typedef struct Color {
    GLfloat r;
    GLfloat g;
    GLfloat b;
} Color;

/** Repraesentiert ein Material */
typedef struct Material {
    /** Lichtanteile */
    Color ka, kd, ks;
    GLfloat shininess;
    /** Reflektions und Refraktionsanteile */
    GLfloat kRefl, kRefr;
} Material;

/** Struct, dass die benoetigten Informationen eines getroffenen Objektes speichert */
typedef struct Hit {
    /** Default Hit */
    GLboolean defaultHit;
    /** Abstand vom Ray-Start zum getroffenen Punkt */
    GLfloat dist;
    /** Position und Normale des getroffenen Punktes */
    vec3 position, normal;
    /** Material des getroffenen Punktes */
    Material material;
    /** Index des getroffenen Objektes */
    objectModels idxObject;
} Hit;

/** 3 Koordinaten */
typedef struct vertices {
    vec3 a;
    vec3 b;
    vec3 c;
} vertices;

/** Indizes die benoetigt werden, um ein Dreieck zu erstellen */
typedef struct faces {
    GLint index1;
    GLint index2;
    GLint index3;
} faces;

/** Stellt ein Dreieck dar, mit 3 Koordinaten, einer Normalen und 2 Kanten(genutzt fuer Trumbore Moeller Algorithmus) */
typedef struct triangleTM {
    vertices vertices;
    vec3 normal;
    vec3 edge1;
    vec3 edge2;
} triangleTM;

/**Struct, dass ein Punktlicht repraesentiert*/
typedef struct pointLight {
    vec3 pos;
    Color color;
    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;
    GLfloat intensity;
    GLboolean active;
} pointLight;


/** ---------------------------------------------------- Objekte ----------------------------------------------------*/

/**Struct fuer ein Objekt, welches mit Dreiecken dargestellt wird*/
typedef struct object {
    GLint vertexCount;
    vec3 *vertices;
    GLint faceCount;
    triangleTM *facesTM;
} object;

/** Struct, dass einen min und Max Wert speichert*/
typedef struct minMax {
    GLfloat min;
    GLfloat max;
} minMax;

/**Struct, dass die 6 Min und Max Koordiantenwerte speichert, um einen Cube zu erzeugen*/
typedef struct cube {
    minMax cornerX;
    minMax cornerY;
    minMax cornerZ;
} cube;

typedef struct corners {
    vec3 topFrontLeft;
    vec3 topFrontRight;
    vec3 bottomFrontLeft;
    vec3 bottomFrontRight;
    vec3 topBackLeft;
    vec3 topBackRight;
    vec3 bottomBackLeft;
    vec3 bottomBackRight;
} corners;

/**Struct, dass eine Bounding Box repraesentiert*/
typedef struct boundingBox {
    GLfloat width;
    GLfloat height;
    GLfloat depth;
    cube cube;
} boundingBox;

/**Aufloesung des Bunny Models*/
typedef enum bunnySize {
    grob,
    mittel,
    fein,
    sehr_fein,
    extrem_fein
}bunnySize;

/**Objekt, in Form einer Kugel (Keine Dreiecke)*/
typedef struct sphere {
    vec3 center;
    GLfloat radius;
} sphere;

/** ------------------------------------------------ MultiThreading Optionen --------------------------------------*/

typedef struct multiThreadRunner {
    /** Breite eines Tiles */
    GLint tileWidth;
    /** Hoehe eines Tiles */
    GLint tileHeight;
    /**Start Pixel der Spalte des Tiles*/
    GLint tileRow;
    /**Start Pixel Reihe des Tiles*/
    GLint tileCol;
} multiThreadRunner;

/**Anzahl der Threads mit denen gerendert werden soll*/
typedef enum multiThreadOptions {
    threads16 = 16,
    threads8 = 8,
    threads4 = 4,
    threads2 = 2,
    noMultiThreading = 1
}multiThreadOptions;

/** Allgemeine Optionen die fuer das MultiThreading verwendet werden sollen */
typedef struct multiThreadOpts {
    /** Multithreading benutzen */
    GLboolean useMultiThreading;
    /** Anzahl vertikale Threads */
    GLint verticalThreads;
    /** Anzahl horizontale Threads */
    GLint horizontalThreads;
    /** Breite eines Tiles */
    GLint tileWidth;
    /** Hoehe eines Tiles */
    GLint tileHeight;
    /** Alle Threads mit ihren Informationen */
    multiThreadRunner *threadArgs;
    /** Alle Thread IDs */
    pthread_t *threadIds;
    /** Multithreading Einstellungen */
    multiThreadOptions threadingOpts;
}multiThreadOpts;

/** ------------------------------------------------ Szene ------------------------------------------------*/

/**Gibt an, aus welcher Richtung wir auf die Szene schauen*/
typedef enum viewMode {
    FRONT,
    BACK,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    ALL = AMOUNT_MODELS
} viewMode;

/**Projektionsebene, auf die die Szene projeziert wird*/
typedef struct projectionPlane {
    viewMode viewMode;
    /**Kameraposition*/
    vec3 cameraPos;
    /**Dimensionen der Ebene*/
    GLfloat viewPortHeight;
    GLfloat viewPortWidth;
    /**Stuetzvektor*/
    vec3 s;
    /**Aufspannvektoren*/
    vec3 u;
    vec3 v;
} projectionPlane;

/**Gibt an, welche und ob eine Bounding Box um den Hasen gerendert werden soll*/
typedef enum boundingBoxState {
    aabb,
    oobb,
    none
} boundingBoxState;

typedef struct scene {
    /** Pixelfarbinformationen fuer das gesamte Bild */
    Color *fb;
    /** Globale Information ueber alle Objekte in der Szene */
    object *allObjects;
    /** Globales Sphaeren Objekt */
    sphere sphere;
    /** Globale Projektionsebene */
    projectionPlane projPlane;
    /** Axis Aligned Bounding Box und Object Oriented Bounding Box des Hasens */
    object boundingBoxes[2];
    /** Status, welche und ob eine Bounding Box gerendert werden soll */
    boundingBoxState bbState;
    /** Status, welche und ob eine Bounding Box gerendert werden soll */
    boundingBoxState lastUsedBB;
    /** Status, ob BoundingBoxes gerendert werden sollen */
    GLboolean showBB;
    /** Globale Informationen ueber alle Punktlichter in der Szene */
    pointLight *pointLights;
    /** Optionen bezueglich des multiThreadings */
    multiThreadOpts multiThreadOpts;
    /** Speicher die Renderzeit der Szene */
    GLfloat renderTime;
} scene;


/** ------------------------------------------- KONSTANTEN FUER FARBEN -------------------------------------------*/
static const Color BLUE = {0.0f, 0.0f, 0.75f};
static const Color AMBIENT_BLUE = {0.0f, 0.0f, 0.25f};
static const Color RED = {0.75f, 0.0f, 0.0f};
static const Color AMBIENT_RED = {0.25f, 0.0f, 0.0f};
static const Color GREEN = {0.0f, 0.75f, 0.0f};
static const Color AMBIENT_GREEN = {0.0f, 0.55f, 0.0f};
static const Color WHITE = {0.75f, 0.75f, 0.75f};
static const Color AMBIENT_WHITE = {0.15f, 0.15f, 0.15f};
static const Color MIRROR_WHITE = {0.1f, 0.1f, 0.1f};
static const Color MIRROR_WHITE_SPEC = {1.0f, 1.0f, 1.0f};
static const Color GREY = {0.2f, 0.2f, 0.2f};
static const Color BACKGROUND_COLOR = {0.0f, 0.0f, 0.0f};

#endif //__RAYTRACER_TYPES_H__
