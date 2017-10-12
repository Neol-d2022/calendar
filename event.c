#include <stdlib.h>
#include <string.h>

#include "strings.h"
#include "event.h"

void EventInit(Event_t *e)
{
    memset(e, 0, sizeof(*e));
}

void EventDeInit(Event_t *e)
{
    if (e->description)
        free(e->description);
    if (e->start)
        free(e->start);
    if (e->end)
        free(e->end);
    if (e->notify)
        free(e->notify);

    EventInit(e);
}

char *EventSetDesc(Event_t *e, const char *desc)
{
    char *p;

    p = nstr(desc);
    if (e->description)
        free(e->description);

    return e->description = p;
}

time_t EventSetStart(Event_t *e, time_t s)
{
    time_t *t;

    t = (time_t *)malloc(sizeof(*t));
    if (!t)
        abort();
    memcpy(t, &s, sizeof(*t));

    if (e->start)
        free(e->start);

    return *(e->start = t);
}

time_t EventSetEnd(Event_t *e, time_t s)
{
    time_t *t;

    t = (time_t *)malloc(sizeof(*t));
    if (!t)
        abort();
    memcpy(t, &s, sizeof(*t));

    if (e->end)
        free(e->end);

    return *(e->end = t);
}

time_t EventSetNotify(Event_t *e, time_t s)
{
    time_t *t;

    t = (time_t *)malloc(sizeof(*t));
    if (!t)
        abort();
    memcpy(t, &s, sizeof(*t));

    if (e->notify)
        free(e->notify);

    return *(e->notify = t);
}
