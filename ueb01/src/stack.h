/**
 * @file
 * Stack-Modul.
 * Implementierung der Datenstruktur Stack
 *
 * Quelle: https://de.wikibooks.org/wiki/Algorithmen_und_Datenstrukturen_in_C/_Stapelspeicher
 *
 * @author Mario Da Graca, Christopher Ploog
 */

#ifndef UEB01_STACK_H
#define UEB01_STACK_H

#include "types.h"

#define SUCCESS 0
#define ERR_INVAL 1
#define ERR_NOMEM 2

#define FALSE 0
#define TRUE 1

typedef struct stack_s stack_t;

int stack_destroy(stack_t *stack);

int stack_empty(stack_t *stack);

stack_t *stack_new(void);

void *stack_pop(stack_t *stack);

int stack_push(stack_t *stack, void *data);

CGPoint *stack_top(stack_t *stack);

#endif //UEB01_STACK_H
