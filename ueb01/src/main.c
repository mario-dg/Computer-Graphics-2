/**
 * @file
 * Cloudy. Initialisierung und Starten der Ergeignisverarbeitung.
 *
 * @author Mario Da Graca, Christopher Ploog
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
    (void) argc;
    (void) argv;

    /* Initialisierung des I/O-Sytems
       (inkl. Erzeugung des Fensters und Starten der Ereignisbehandlung). */
    if (!initAndStartIO ("Cloudy", 1000, 900))
    {
        fprintf (stderr, "Initialisierung fehlgeschlagen!\n");
        return 1;
    }
    else
    {
        /* Alles geklappt */
        return 0;
    }
}