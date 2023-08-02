/**
 * @file
 * Splineflaechen-Modul.
 * Das Modul kapselt die Berrechnungen, welche für die Splineflachen Funktionalitaet
 * benoetigt werden
 *
 * @author Mario da Graca, Christopher Ploog
 */

#ifndef UEB02_SPLINE_SURFACE_H
#define UEB02_SPLINE_SURFACE_H
GLfloat *splineSurface(GLint amountControlVertices, GLint amountInterpolVertices, GLfloat *verts, calculationMode mode);
#endif //UEB02_SPLINE_SURFACE_H
