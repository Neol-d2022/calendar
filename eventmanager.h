#ifndef _EVENTMANAGER_H_LOADED
#define _EVENTMANAGER_H_LOADED

#include <stdbool.h>

#include "event.h"

typedef struct
{
    unsigned long long count;
    unsigned long long capacity;
    Event_t *storage;
    Event_t ***indexes;
    bool indexed;
} EventManager_t;

typedef struct
{
    unsigned long long length;
    Event_t **results;
} EventManagerSearchResultSet_t;

void EMInit(EventManager_t *em);
void EMDeInit(EventManager_t *em);
unsigned long long EMAdd(EventManager_t *em, const Event_t *e);
int EMRemove(EventManager_t *em, unsigned long long uuid);

void EMSearchStart(EventManager_t *em, const time_t *start, const time_t *end, EventManagerSearchResultSet_t *r);
void EMSearchEnd(EventManager_t *em, const time_t *start, const time_t *end, EventManagerSearchResultSet_t *r);
void EMSearchNotify(EventManager_t *em, const time_t *start, const time_t *end, EventManagerSearchResultSet_t *r);
void EMSRS_DeInit(EventManagerSearchResultSet_t *r);

#endif
