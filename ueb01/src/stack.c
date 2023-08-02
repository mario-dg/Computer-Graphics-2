/**
 * @file
 * Stack-Modul.
 * Implementierung der Datenstruktur Stack
 *
 * Quelle: https://de.wikibooks.org/wiki/Algorithmen_und_Datenstrukturen_in_C/_Stapelspeicher
 *
 * @author Mario Da Graca, Christopher Ploog
 */

#include "stack.h"
#include <stdlib.h>

/**
 * Zerstoert Stack
 * @param stack zu zerstoerender Stack
 * @return Erflog (Nein=0; Ja=1)
 */
int stack_destroy(stack_t *stack) {
    if (stack == NULL) {
        return ERR_INVAL;
    }
    while (stack->top != NULL) {
        struct stack_frame_s *frame = stack->top;
        stack->top = frame->next;
        free(frame);
    }
    free(stack);
    return SUCCESS;
}

/**
 * Ueberprueft ob Stack leer ist
 * @param stack zu pruefender Stack
 * @return Boolean (Ja=true; Nein=False)
 */
int stack_empty(stack_t *stack) {
    if (stack == NULL || stack->top == NULL) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * Erstellen eines neuen Stacks
 * @return Pointer auf neuen Stack
 */
stack_t *stack_new(void) {
    stack_t *stack = malloc(sizeof(*stack));
    if (stack == NULL) {
        return NULL;
    }
    stack->top = NULL;
    return stack;
}

/**
 * Gibt oberstes Element vom Stack zurueck und entfernt dieses
 * @param stack zu bearbeitender Stack
 * @return Oberstes Element
 */
void *stack_pop(stack_t *stack) {
    if (stack == NULL || stack->top == NULL) {
        return NULL;
    }
    struct stack_frame_s *frame = stack->top;
    void *data = frame->data;
    stack->top = frame->next;
    free(frame);
    return data;
}

/**
 * Packt ein Element auf den Stack
 * @param stack zu bearbeitender Stack
 * @param data neue Daten
 * @return Erfolg
 */
int stack_push(stack_t *stack, void *data) {
    if (stack == NULL) {
        return ERR_INVAL;
    }
    struct stack_frame_s *frame = malloc(sizeof(*frame));
    if (frame == NULL) {
        return ERR_NOMEM;
    }
    frame->data = data;
    frame->next = stack->top;
    stack->top = frame;
    return SUCCESS;
}

/**
 * Liefert Oberstes Element vom Stack, ohne dieses zu entfernen
 * @param stack zu bearbeitender Stack
 * @return oberstes Element vom Stack
 */
CGPoint *stack_top(stack_t *stack) {
    if (!stack || !stack->top) {
        return NULL;
    }
    struct stack_frame_s *frame = stack->top;
    void *data = frame->data;
    return data;
}
