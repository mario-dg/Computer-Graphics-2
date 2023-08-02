/**
 * @file
 * Implementiert Multithreading und teilt jedem Runner seinen Bereich der Szene
 * zu, die dieser rendern soll
 *
 * @author Christopher Ploog, Mario da Graca
 */

#include "multiThreading.h"

/**
 * Erstellt einen Multithread Runner fuer ein Bereich der zu rendernden Szene
 * @param tileCol Start Pixel Reihe des Tiles
 * @param tileWidth Breite eines Tiles
 * @param tileRow Start Pixel der Spalte des Tiles
 * @param tileHeight Hoehe eines Tiles
 * @return
 */
static multiThreadRunner multiThreading_initRunner(GLint tileCol, GLint tileWidth, GLint tileRow, GLint tileHeight) {
    multiThreadRunner result;
    result.tileWidth = tileWidth;
    result.tileHeight = tileHeight;

    result.tileRow = tileRow;
    result.tileCol = tileCol;

    return result;
}

/**
 * Initialisiert die Argumente fuer einen Thread
 * @param scene Aktuelle Szene
 * @param horiThreads Anzahl der horizontalen Threads
 * @param vertThreads Anzahl der vertikalen Threads
 * */
static void multiThreading_initThread(scene *scene, GLint horiThreads, GLint vertThreads) {
    //Allgemeine Infos ueber das Szene und das zu rendernde Bild
    scene->multiThreadOpts.useMultiThreading = GL_TRUE;
    scene->multiThreadOpts.horizontalThreads = horiThreads;
    scene->multiThreadOpts.tileWidth = DEFAULT_WINDOW_WIDTH / scene->multiThreadOpts.horizontalThreads;
    scene->multiThreadOpts.verticalThreads = vertThreads;
    scene->multiThreadOpts.tileHeight = DEFAULT_WINDOW_HEIGHT / scene->multiThreadOpts.verticalThreads;

    //Speicher reservieren
    scene->multiThreadOpts.threadArgs = (multiThreadRunner *) calloc(scene->multiThreadOpts.horizontalThreads *
                                                                     scene->multiThreadOpts.verticalThreads,
                                                                     sizeof(multiThreadRunner));

    //Jedem Runner seinen Bereich der Szene zuordnen und im Array speichern
    for (int y = 0; y < scene->multiThreadOpts.verticalThreads; ++y) {
        for (int x = 0; x < scene->multiThreadOpts.horizontalThreads; ++x) {
            scene->multiThreadOpts.threadArgs[OFFSET2D(scene->multiThreadOpts.horizontalThreads, x, y)] =
                    multiThreading_initRunner(x * scene->multiThreadOpts.tileWidth, scene->multiThreadOpts.tileWidth,
                                              y * scene->multiThreadOpts.tileHeight, scene->multiThreadOpts.tileHeight);
        }
    }
}

void multiThreading_setupThreading(scene *scene) {
    GLint horiThreads;
    GLint vertThreads;
    //Thread Anzahl in vertikale und horizontale Richtung aufteilen
    switch (scene->multiThreadOpts.threadingOpts) {
        case threads16:
            horiThreads = 4;
            vertThreads = 4;
            break;
        case threads8:
            horiThreads = 4;
            vertThreads = 2;
            break;
        case threads4:
            horiThreads = 4;
            vertThreads = 1;
            break;
        case threads2:
            horiThreads = 2;
            vertThreads = 1;
            break;
        case noMultiThreading:
        default:
            horiThreads = 0;
            vertThreads = 0;
            break;
    }

    if(horiThreads >= 1 && vertThreads >= 1){
        multiThreading_initThread(scene, horiThreads, vertThreads);
        scene->multiThreadOpts.useMultiThreading = GL_TRUE;
    } else {
        scene->multiThreadOpts.useMultiThreading = GL_FALSE;
    }
}
