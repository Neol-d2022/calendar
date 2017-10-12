#ifndef _LINKEDLIST_H_LOADED
#define _LINKEDLIST_H_LOADED

#include <stddef.h>

typedef struct _linked_list_node_struct
{
    void *v;
    struct _linked_list_node_struct *next;
} LinkedNode_t;

typedef struct
{
    LinkedNode_t *head;
    size_t count;
} Linked_t;

void LinkedInit(Linked_t *l);
void LinkedDeInit(Linked_t *l);

void LinkedQuickInsert(Linked_t *l, LinkedNode_t ***ppp, void *v);
void LinkedIterate(const Linked_t *l, void *param, void (*pf)(void *data, void *param));

#endif
