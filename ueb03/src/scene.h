#ifndef __SCENE_H__
#define __SCENE_H__
#include "types.h"
/**
 * @file
 * Schnittstelle des Darstellungs-Moduls.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 *
 * @author Mario da Graca, Leonhard Brandes
 */

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar
 */
void drawScene (void);

/**
 * Aktualisiert die Normalen fuer alle Kugeln
 */
void updateNormals(void);

/**
 * Aktualisiert das Vertex-Array, wenn die Aufloesung des Mesh veraendert wird
 * @param state ob vergroessert oder verkleinert wird
 */
void updateVertexArray(expandShrinkVertices state);

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene (void);

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode (void);


/**
 * Setzt den Kameraradius auf den uebergebenen Wert
 * @param rad Radius der Kamera
 */
void setCameraRadius(GLfloat rad);

/**
 * Setzt den Polarwinkeln der Kamera im Kugelkoordinatensystem auf den uebergebenen Wert
 * @param theta Wert des Winkels im Bogenmass
 */
void setCameraTheta(GLfloat theta);

/**
 * Setzt den Azimutwinkel der Kamera im Kugelkoordinatensystem auf den uebergebenen Wert
 * @param phi Wert des Winkels im Bogenmass
 */
void setCameraPhi(GLfloat phi);

/**
 * liefert den Azimutwinkel der Kamera im Kugelkoordinatensystem
 * @return Azimutwinkel der Kamera im Kugelkoordinatensystem im Bogenmass
 */ 
GLfloat getCameraPhi(void);

/**
 * liefert den Polarwinkel der Kamera im Kugelkoordinatensystem
 * @return Polarwinkel der Kamera im Kugelkoordinatensystem im Bogenmass
 */ 
GLfloat getCameraTheta(void);

/**
 * liefert den Radius der Kamera im Kugelkoordinatensystem
 * @return Radius der Kamera im Kugelkoordinatensystem
 */ 
GLfloat getCameraRadius(void);

/**
 * Schaltet das ANzeigen der Normalen um
 */ 
void toggleNormal(void);

/**
 * Schaltet das Anzeigen der Kugeln um
 */
void toggleSpheres(void);
/**
 * Schaltet das Anzeigen der interpolierten Kugeln um
 */
void toggleInterpolSpheres(void);
/**
 * Updated die anzahl der interpolierten Punkte (Weichheit)
 * @param v verringerung oder steigerung um 1
 */
void updateSplineVertices(GLint v);

/**
 * Schaltet das Anzeigen der Hilfe um
 */ 
void toggleShowHelp(void);

/**
 * gibt den in der Scene reservierten Speicher wieder frei
 */
void freeAllocatedMem(void);

void updateScene(void);

void fillIndexArray(GLint amountIndices);
void updateIndexArray(GLint amountIndices);

/**
 * Aktualisiert die Hoehe des ausgewaelten Verts
 * @param direction
 */
void updateSelectedHeight(int direction);
/**
 * Schaltet zwischen den verschiedenen Texturen hin und her
 */
void nextTexture();

/**
 * Schaltet den Gewonnen Status auf v
 * @param v eingegebener Gewonnen Status
 */
void toggleWin(GLboolean v);

/**
 * Schaltet den Verloren Status auf v
 * @param v eingegebener Verloren Status
 */
void toggleLoss(GLboolean v);
#endif
