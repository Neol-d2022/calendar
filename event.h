#ifndef _EVENT_H_LOADED
#define _EVENT_H_LOADED

#include <time.h>

typedef struct
{
    unsigned long long uuid;
    char *description;
    time_t *start;
    time_t *end;
    time_t *notify;
} Event_t;

void EventInit(Event_t *e);
void EventDeInit(Event_t *e);
char *EventSetDesc(Event_t *e, const char *desc);
time_t EventSetStart(Event_t *e, time_t s);
time_t EventSetEnd(Event_t *e, time_t s);
time_t EventSetNotify(Event_t *e, time_t s);

#endif
