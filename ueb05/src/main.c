/**
 * @file
 * Hauptprogramm. Initialisierung und Starten der Ergeignisverarbeitung.
 *
 * Bestandteil eines Beispielprogramms fuer Textausgabe mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>

/* ---- Eigene Header einbinden ---- */
#include "io.h"

/**
 * Hauptprogramm.
 * Initialisierung und Starten der Ereignisbehandlung.
 * @param argc Anzahl der Kommandozeilenparameter (In).
 * @param argv Kommandozeilenparameter (In).
 * @return Rueckgabewert im Fehlerfall ungleich Null.
 */
int
main (int argc, char **argv)
{
	(void)argc;
	(void)argv;
  /* Initialisierung des I/O-Sytems
     (inkl. Erzeugung des Fensters und Starten der Ereignisbehandlung). */
  if (!initAndStartIO
      ("Raytracer", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT ))
    {
      fprintf (stderr, "Initialisierung fehlgeschlagen!\n");
      return 1;
    }
  else
    {
      return 0;
    }
}
