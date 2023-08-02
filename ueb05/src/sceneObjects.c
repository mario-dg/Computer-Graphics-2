/**
 * @file
 * Erstellt Default Objekte fuer die Szene, setzt Materialien und baut die Szene auf
 *
 * @author Christopher Ploog, Mario da Graca
 */

#include "sceneObjects.h"
#include "loadObj.h"
#include "boundingBox.h"


/**---------------------------------------- LOCAL FUNCTION IMPLEMENTATION --------------------------------------*/

/**
 * Berechnet die AABB und die OBB von einem Objekt in der Szene
 * @param scene Aktuelle Szene
 * @param idx Index des Objektes, desses Bounding Boxes generiert werden soll
 */
static void sceneObjects_initBoundingBoxes(scene *scene, GLint idx, vec3 bunnyTranslation) {
    //Axis Aligned Bounding Box
    corners aabbBox;
    boundingBox aabb = boundingBox_calculateAABB(scene->allObjects[idx], &aabbBox);
    scene->boundingBoxes[0] = boundingBox_createObjectFromBoundingBox(aabbBox);

    //Object Oriented Bounding Box
    corners oobbBox;
    boundingBox_createOOBFromAABB(aabb, scene->allObjects[idx], &oobbBox, bunnyTranslation);
    scene->boundingBoxes[1] = boundingBox_createObjectFromBoundingBox(oobbBox);

}


/**
 * Laedt ein Quadrat mit Seitenlaenge 1 um den Ursprung
 * @param model Seite der Box
 * @param translation Position der Seite
 * @param rotation Rotation der Seite
 * @param scale Skalierung der Seite
 */
static void sceneObjects_renderPlane(scene* scene, objectModels model, vec3 translation, vec3 rotation, GLfloat scale) {
    scene->allObjects[model] = loadObj_readFile("plane.obj", translation, rotation, scale);
}

/**
 * Fuegt eine Box der Szene hinzu, die alle anderen Objekte beinhaltet,
 * laesst die Seite aus, von der aus geguckt wird
 * @param scale Skalierung der Box
 */
void sceneObjects_loadBox(scene scene) {
    //Box skalierung
    GLfloat scale = 2.0f;
    //Box translations
    vec3 lowerTrans = {0.0f, -scale / 1.99999f, 0.0f};
    vec3 leftTrans = {-scale / 1.99999f, 0.0f, 0.0f};
    vec3 rightTrans = {scale / 1.99999f, 0.0f, 0.0f};
    vec3 rearTrans = {0.0f, 0.0f, -scale / 1.99999f};
    vec3 upperTrans = {0.0f, scale / 1.99999f, 0.0f};
    vec3 frontTrans = {0.0f, 0.0f, scale / 1.99999f};

    //Box rotations
    vec3 lowerRot = {0, 0, 0};
    vec3 leftRot = {0, 0, -90};
    vec3 rightRot = {0, 0, 90};
    vec3 rearRot = {90, 0, 0};
    vec3 upperRot = {180, 0, 0};
    vec3 frontRot = {-90, 0, 0};

    //Alle Waende laden
    sceneObjects_renderPlane(&scene, UPPER_WALL, upperTrans, upperRot, scale);
    sceneObjects_renderPlane(&scene, LOWER_WALL, lowerTrans, lowerRot, scale);
    sceneObjects_renderPlane(&scene, RIGHT_WALL, rightTrans, rightRot, scale);
    sceneObjects_renderPlane(&scene, LEFT_WALL, leftTrans, leftRot, scale);
    sceneObjects_renderPlane(&scene, FRONT_WALL, frontTrans, frontRot, scale);
    sceneObjects_renderPlane(&scene, REAR_WALL, rearTrans, rearRot, scale);
}

/**
 * Laedt den teiltransparenten Wuerfel
 * @param scene aktuelle Szene
 */
static void sceneObjects_loadCube(scene * scene){
    vec3 cubeTranslation = {-1.2f * CUBE_SCALE, -0.79999f, 1.2f * CUBE_SCALE};
    vec3 cubeRotation = {0.0f, -33.0f, 0.0f};
    scene->allObjects[CUBE] = loadObj_readFile("cube.obj", cubeTranslation, cubeRotation, CUBE_SCALE);
}

/**
 * Laedt den Spiegel
 * @param scene aktuelle Szene
 */
static void sceneObjects_loadMirror(scene *scene) {
    if (scene->projPlane.viewMode != BACK) {
        scene->allObjects[MIRROR] = loadObj_readFile("mirror.obj", (vec3) {0, 0, 0}, (vec3) {0, 0, 0}, 1.0f);
    } else {
        scene->allObjects[MIRROR] = sceneObjects_initDefaultModel();
    }
}

/**
 * Laedt den Hasen
 * @param scene aktuelle Szene
 * @param size Feinheit des Hasens
 */
static void sceneObjects_loadBunny(scene * scene, bunnySize size){
    char *fileName;
    vec3 bunnyTranslation = {0.55f, -0.999f, -0.075f};
    vec3 bunnyRotation = {0.0f, 45.0f, 0.0f};
    GLfloat scale = BUNNY_SCALE;
    switch (size) {
        case grob:
            fileName = "bunny-grob.obj";
            glm_vec3_copy((vec3){0.0f, -45.0f, 0.0f}, bunnyRotation);
            break;
        case mittel:
            fileName = "bunny-med.obj";
            break;
        case fein:
            fileName = "bunny-fein.obj";
            break;
        case sehr_fein:
            fileName = "bunny-sehrFein.obj";
            break;
        case extrem_fein:
            fileName = "bunny-extreme.obj";
            break;
        default:
            printf("Unknown Bunny!\n");
            exit(1);
    }

    scene->allObjects[BUNNY] = loadObj_readFile(fileName, bunnyTranslation, bunnyRotation, scale);

    sceneObjects_initBoundingBoxes(scene, BUNNY, bunnyTranslation);

    scene->allObjects[BOUNDING_BOX] = scene->boundingBoxes[scene->lastUsedBB];
}

/**
 * Laedt die Sphaere
 * @param scene aktuelle Szene
 */
static void sceneObjects_loadSphere(scene *scene){
    glm_vec3_copy((vec3) {0.0f, -0.799f, -0.65f}, scene->sphere.center);
    scene->sphere.radius = SPHERE_RADIUS;
    scene->allObjects[SPHERE] = sceneObjects_initDefaultModel();
}

/**---------------------------------------- GLOBAL FUNCTION IMPLEMENTATION --------------------------------------*/

object sceneObjects_initDefaultModel(void) {
    object result;

    result.vertexCount = 0;
    result.faceCount = 0;
    result.vertices = NULL;
    result.facesTM = NULL;

    return result;
}

void sceneObjects_initModels(scene *scene) {
    /*---------------------------- WUERFEL ----------------------------*/
    sceneObjects_loadCube(scene);

    /*---------------------------- SPIEGEL ----------------------------*/
    sceneObjects_loadMirror(scene);

    /*---------------------------- HASE ----------------------------*/
    sceneObjects_loadBunny(scene, grob);

    /*---------------------------- KUGEL ----------------------------*/
    sceneObjects_loadSphere(scene);

    /*---------------------------- BOX ----------------------------*/
    sceneObjects_loadBox(*scene);
}

void sceneObjects_setViewDir(scene * scene, viewMode mode) {
    scene->projPlane.viewMode = mode;

    switch (scene->projPlane.viewMode) {
        case FRONT:
            //Kameraposition
            glm_vec3_copy((vec3) {0.0f, 0.5f, 4.0f}, scene->projPlane.cameraPos);

            //Vektoren, die die Ebene aufspannen
            //abhaengig von der eingestellten Aufloesung
            glm_vec3_copy((vec3) {scene->projPlane.viewPortWidth / xRes, 0.0f, 0.0f}, scene->projPlane.u);
            glm_vec3_copy((vec3) {0.0f, scene->projPlane.viewPortHeight / yRes, 0.0f}, scene->projPlane.v);


            //Startpunkt der Projetionsebene
            scene->projPlane.s[0] = -scene->projPlane.viewPortWidth / 2.0f;
            scene->projPlane.s[1] = -scene->projPlane.viewPortHeight / 2.0f;
            scene->projPlane.s[2] = 1.0f;
            break;
        case BACK:
            //Kameraposition
            glm_vec3_copy((vec3) {0.0f, 0.5f, -4.0f}, scene->projPlane.cameraPos);

            //Vektoren, die die Ebene aufspannen
            //abhaengig von der eingestellten Aufloesung
            glm_vec3_copy((vec3) {-scene->projPlane.viewPortWidth / xRes, 0.0f, 0.0f}, scene->projPlane.u);
            glm_vec3_copy((vec3) {0.0f, scene->projPlane.viewPortHeight / yRes, 0.0f}, scene->projPlane.v);


            //Startpunkt der Projetionsebene
            scene->projPlane.s[0] = scene->projPlane.viewPortWidth / 2.0f;
            scene->projPlane.s[1] = -scene->projPlane.viewPortHeight / 2.0f;
            scene->projPlane.s[2] = -1.0f;
            break;
        case TOP:
            //Kameraposition
            glm_vec3_copy((vec3) {0.1f, 4.0f, 0.0f}, scene->projPlane.cameraPos);

            //Vektoren, die die Ebene aufspannen
            //abhaengig von der eingestellten Aufloesung
            glm_vec3_copy((vec3) {scene->projPlane.viewPortWidth / xRes, 0.0f, 0.0f}, scene->projPlane.u);
            glm_vec3_copy((vec3) {0.0f, 0.0f, -scene->projPlane.viewPortHeight / yRes}, scene->projPlane.v);


            //Startpunkt der Projetionsebene
            scene->projPlane.s[0] = -scene->projPlane.viewPortWidth / 2.0f;
            scene->projPlane.s[1] = 1.0f;
            scene->projPlane.s[2] = scene->projPlane.viewPortHeight / 2.0f;
            break;
        case BOTTOM:
            //Kameraposition
            glm_vec3_copy((vec3) {0.1f, -4.0f, 0.0f}, scene->projPlane.cameraPos);

            //Vektoren, die die Ebene aufspannen
            //abhaengig von der eingestellten Aufloesung
            glm_vec3_copy((vec3) {scene->projPlane.viewPortWidth / xRes, 0.0f, 0.0f}, scene->projPlane.u);
            glm_vec3_copy((vec3) {0.0f, 0.0f, -scene->projPlane.viewPortHeight / yRes}, scene->projPlane.v);


            //Startpunkt der Projetionsebene
            scene->projPlane.s[0] = -scene->projPlane.viewPortWidth / 2.0f;
            scene->projPlane.s[1] = -1.0f;
            scene->projPlane.s[2] = scene->projPlane.viewPortHeight / 2.0f;
            break;
        case LEFT:
            //Kameraposition
            glm_vec3_copy((vec3) {-4.0f, 0.1f, 0.0f}, scene->projPlane.cameraPos);

            //Vektoren, die die Ebene aufspannen
            //abhaengig von der eingestellten Aufloesung
            glm_vec3_copy((vec3) {0.0f, 0.0f, scene->projPlane.viewPortHeight / yRes}, scene->projPlane.u);
            glm_vec3_copy((vec3) {0.0f, scene->projPlane.viewPortHeight / yRes, 0.0f}, scene->projPlane.v);


            //Startpunkt der Projetionsebene
            scene->projPlane.s[0] = -scene->projPlane.viewPortWidth / 2.0f;
            scene->projPlane.s[1] = -scene->projPlane.viewPortHeight / 2.0f;
            scene->projPlane.s[2] = -1.0f;
            break;
        case RIGHT:
            //Kameraposition
            glm_vec3_copy((vec3) {4.0f, 0.1f, 0.0f}, scene->projPlane.cameraPos);

            //Vektoren, die die Ebene aufspannen
            //abhaengig von der eingestellten Aufloesung
            glm_vec3_copy((vec3) {0.0f, 0.0f, -scene->projPlane.viewPortHeight / yRes}, scene->projPlane.u);
            glm_vec3_copy((vec3) {0.0f, scene->projPlane.viewPortHeight / yRes, 0.0f}, scene->projPlane.v);


            //Startpunkt der Projetionsebene
            scene->projPlane.s[0] = scene->projPlane.viewPortWidth / 2.0f;
            scene->projPlane.s[1] = -scene->projPlane.viewPortHeight / 2.0f;
            scene->projPlane.s[2] = 1.0f;
            break;
        default:
            break;
    }
}

void sceneObjects_initPointLights(scene *scene) {
    scene->pointLights = (pointLight *) calloc(AMOUNT_LIGHTS, sizeof(struct pointLight));

    /*------- Punktlicht 1 -------*/
    //Position
    scene->pointLights[0].pos[0] = -0.25f;
    scene->pointLights[0].pos[1] = 0.2f;
    scene->pointLights[0].pos[2] = -0.5f;
    //Farbe
    scene->pointLights[0].color.r = 0.9f;
    scene->pointLights[0].color.g = 0.9f;
    scene->pointLights[0].color.b = 0.9f;
    //Konstante Faktoren
    scene->pointLights[0].constant = 1.0f;
    scene->pointLights[0].linear = 0.09f;
    scene->pointLights[0].quadratic = 0.032f;
    //Intensitaet des Lichtes
    scene->pointLights[0].intensity = 0.9f;
    //Aktiv
    scene->pointLights[0].active = GL_TRUE;


    /*------- Punktlicht 2 -------*/
    //Position
    scene->pointLights[1].pos[0] = 0.2f;
    scene->pointLights[1].pos[1] = 0.55f;
    scene->pointLights[1].pos[2] = 0.75f;
    //Farbe
    scene->pointLights[1].color.r = 1.0f;
    scene->pointLights[1].color.g = 1.0f;
    scene->pointLights[1].color.b = 0.9f;
    //Konstante Faktoren
    scene->pointLights[1].constant = 1.0f;
    scene->pointLights[1].linear = 0.09f;
    scene->pointLights[1].quadratic = 0.032f;
    //Intensitaet des Lichtes
    scene->pointLights[1].intensity = 0.9f;
    //Aktiv
    scene->pointLights[1].active = GL_TRUE;
}

void sceneObjects_setHitObjectMaterial(scene scene, Hit *hitObject) {
    switch (hitObject->idxObject) {
        case BUNNY:
            hitObject->material.ka = AMBIENT_GREEN;
            hitObject->material.kd = GREEN;
            hitObject->material.ks = GREEN;
            hitObject->material.shininess = 2.0f;
            hitObject->material.kRefr = 0.0f;
            hitObject->material.kRefl = 0.2f;
            break;
        case SPHERE:
            hitObject->material.ka = AMBIENT_BLUE;
            hitObject->material.kd = BLUE;
            hitObject->material.ks = BLUE;
            hitObject->material.shininess = 35.0f;
            hitObject->material.kRefr = 0.0f;
            hitObject->material.kRefl = 0.01f;
            break;
        case BOUNDING_BOX:
            hitObject->material.ka = GREY;
            hitObject->material.kd = GREY;
            hitObject->material.ks = GREY;
            hitObject->material.shininess = 1.0f;
            hitObject->material.kRefr = 0.8f;
            hitObject->material.kRefl = 0.1f;
            break;
        case CUBE:
            hitObject->material.ka = AMBIENT_RED;
            hitObject->material.kd = RED;
            hitObject->material.ks = RED;
            hitObject->material.shininess = 4.0f;
            hitObject->material.kRefr = 0.6f;
            hitObject->material.kRefl = 0.1f;
            break;
        case MIRROR:
            hitObject->material.ka = MIRROR_WHITE;
            hitObject->material.kd = MIRROR_WHITE;
            hitObject->material.ks = MIRROR_WHITE_SPEC;
            hitObject->material.shininess = 25.0f;
            hitObject->material.kRefr = 0.0f;
            hitObject->material.kRefl = 0.7f;
            break;
        case REAR_WALL:
        case FRONT_WALL:
        case UPPER_WALL:
        case LOWER_WALL:
        case LEFT_WALL:
        case RIGHT_WALL:
            hitObject->material.ka = AMBIENT_WHITE;
            hitObject->material.kd = WHITE;
            hitObject->material.ks = AMBIENT_WHITE;
            hitObject->material.shininess = 1.0f;
            hitObject->material.kRefr = 0.0f;
            hitObject->material.kRefl = 0.01f;
            break;
        default:
            break;
    }

}