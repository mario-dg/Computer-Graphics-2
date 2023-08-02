#ifndef RAYTRACER_TRUMBOREMOELLER_H
#define RAYTRACER_TRUMBOREMOELLER_H
#include "types.h"

/**
 * Prueft, ob ein Strahl ein Dreieck trifft
 * !!! Effiziente Implementierung SEHR WICHTIG !!!
 * @param Ray Strahl
 * @param triangle Dreieck
 * @return wenn das Dreieck nicht getroffen wurde (Default Werte)
 *         wenn das Dreieck getroffen wurde (Infos ueber nahesten Punkt) ->
 *         (Distanz zum getroffenen Punkt,
 *          Position des getroffenen Punktes)
 */
Hit trumboreMoeller_rayTriangleIntersection(Ray, triangleTM);

#endif //RAYTRACER_TRUMBOREMOELLER_H
