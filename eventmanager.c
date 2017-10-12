#include <stdlib.h>
#include <string.h>

#include "eventmanager.h"

#define INITIAL_CAPACITY 8
#define INDEXES_LENGTH 3

static int _cmpByUuid(const void *a, const void *b);
static int _cmpEventIndexStart(const void *a, const void *b);
static int _cmpEventIndexEnd(const void *a, const void *b);
static int _cmpEventIndexNotify(const void *a, const void *b);

static int (*pf_indexCmpFunc[INDEXES_LENGTH])(const void *a, const void *b) = {
    _cmpEventIndexStart,
    _cmpEventIndexEnd,
    _cmpEventIndexNotify,
};

void EMInit(EventManager_t *em)
{
    memset(em, 0, sizeof(*em));
    em->capacity = INITIAL_CAPACITY;
    em->storage = (Event_t *)malloc(sizeof(*(em->storage)) * em->capacity);
    if (!em->storage)
        abort();
    em->indexes = (Event_t ***)malloc(sizeof(*(em->indexes)) * INDEXES_LENGTH);
    if (!em->indexes)
        abort();
    memset(em->indexes, 0, sizeof(*(em->indexes)) * INDEXES_LENGTH);
}

void EMDeInit(EventManager_t *em)
{
    unsigned long long i;

    for (i = 0; i < em->count; i += 1)
        EventDeInit((em->storage) + i);
    free(em->storage);
    for (i = 0; i < INDEXES_LENGTH; i += 1)
        if ((em->indexes)[i])
            free((em->indexes)[i]);
    free(em->indexes);
    memset(em, 0, sizeof(*em));
}

static void _EMExpend(EventManager_t *em)
{
    unsigned long long c;
    Event_t *e;

    c = (em->capacity) << 1;
    e = (Event_t *)realloc(em->storage, sizeof(*(em->storage)) * c);
    if (!e)
        abort();
    em->capacity = c;
    if ((size_t)e != (size_t)(em->storage))
    {
        em->indexed = false;
        em->storage = e;
    }
}

static void _EMRefreshIndexed(EventManager_t *em)
{
    unsigned long long i, j;

    if (em->count == 0)
        return;

    for (i = 0; i < INDEXES_LENGTH; i += 1)
    {
        if ((em->indexes)[i])
            free((em->indexes)[i]);
        (em->indexes)[i] = (Event_t **)malloc(sizeof(*((em->indexes)[i])) * em->count);
        if (!((em->indexes)[i]))
            abort();
    }

    for (i = 0; i < INDEXES_LENGTH; i += 1)
    {
        for (j = 0; j < em->count; j += 1)
            (em->indexes)[i][j] = em->storage + j;
        qsort((em->indexes)[i], em->count, sizeof(*((em->indexes)[i])), pf_indexCmpFunc[i]);
    }

    em->indexed = true;
}

unsigned long long EMAdd(EventManager_t *em, const Event_t *e)
{
    unsigned long long uuid = 1;
    Event_t *f;

    if (em->count < em->capacity)
    {
    }
    else if (em->count == em->capacity)
        _EMExpend(em);
    else
        abort();

    if (em->count > 0)
        uuid = em->storage[em->count - 1].uuid + 1;

    f = em->storage + em->count;
    EventInit(f);
    EventSetDesc(f, e->description);
    if (e->start)
        EventSetStart(f, *(e->start));
    if (e->end)
        EventSetEnd(f, *(e->end));
    if (e->notify)
        EventSetNotify(f, *(e->notify));
    f->uuid = uuid;
    em->count += 1;
    em->indexed = false;

    return uuid;
}

int EMRemove(EventManager_t *em, unsigned long long uuid)
{
    Event_t k;
    Event_t *f;
    size_t d;

    k.uuid = uuid;
    f = (Event_t *)bsearch(&k, em->storage, em->count, sizeof(*(em->storage)), _cmpByUuid);

    if (f)
    {
        EventDeInit(f);
        if (em->count > 1)
        {
            d = ((size_t)f - (size_t)(em->storage)) / sizeof(*(em->storage));
            memcpy(f, f + 1, em->count - 1 - d);
        }
        em->count -= 1;
        em->indexed = false;
        return 0;
    }
    else
        return 1;
}

static time_t **_EventGetTimeByIndexId(Event_t *e, unsigned int indexId)
{
    switch (indexId)
    {
    case 0:
        return &(e->start);
    case 1:
        return &(e->end);
    case 2:
        return &(e->notify);
    default:
        abort();
    }
}

static void _EMSearchIndexesTimeBothInclusive(EventManager_t *em, const time_t *start, const time_t *end, EventManagerSearchResultSet_t *r, unsigned int indexId)
{
    Event_t k[2];
    Event_t **index, *keys[2], **p, **q;
    size_t s, e, l;

    if (!em->indexed)
        _EMRefreshIndexed(em);

    k[0].uuid = 0;
    k[1].uuid = em->storage[em->count - 1].uuid + 1;
    *_EventGetTimeByIndexId(k + 0, indexId) = (time_t *)start;
    *_EventGetTimeByIndexId(k + 1, indexId) = (time_t *)end;

    index = (Event_t **)malloc(sizeof(*index) * (em->count + 2));
    if (!index)
        abort();

    memcpy(index, em->indexes[indexId], sizeof(*(em->indexes[indexId])) * em->count);
    index[em->count + 0] = k + 0;
    index[em->count + 1] = k + 1;
    qsort(index, em->count + 2, sizeof(*index), pf_indexCmpFunc[indexId]);
    keys[0] = k + 0;
    keys[1] = k + 1;
    p = (Event_t **)bsearch(keys + 0, index, em->count + 2, sizeof(*index), pf_indexCmpFunc[indexId]);
    q = (Event_t **)bsearch(keys + 1, index, em->count + 2, sizeof(*index), pf_indexCmpFunc[indexId]);
    s = ((size_t)p - (size_t)index) / sizeof(*index);
    e = ((size_t)q - (size_t)index) / sizeof(*index);
    l = e - s - 1;
    s++;
    memset(r, 0, sizeof(*r));
    r->length = l;
    if (l > 0)
    {
        r->results = (Event_t **)malloc(sizeof(*(r->results)) * l);
        if (!r->results)
            abort();
        memcpy(r->results, index + s, l * sizeof(*(r->results)));
    }

    free(index);
}

void EMSearchStart(EventManager_t *em, const time_t *start, const time_t *end, EventManagerSearchResultSet_t *r)
{
    _EMSearchIndexesTimeBothInclusive(em, start, end, r, 0);
}

void EMSearchEnd(EventManager_t *em, const time_t *start, const time_t *end, EventManagerSearchResultSet_t *r)
{
    _EMSearchIndexesTimeBothInclusive(em, start, end, r, 1);
}

void EMSearchNotify(EventManager_t *em, const time_t *start, const time_t *end, EventManagerSearchResultSet_t *r)
{
    _EMSearchIndexesTimeBothInclusive(em, start, end, r, 2);
}

void EMSRS_DeInit(EventManagerSearchResultSet_t *r)
{
    if (r->results)
        free(r->results);
}

/*************
 *           *
 * Comparers *
 *           *
 *************/

static int _cmpByUuid(const void *a, const void *b)
{
    Event_t *c = (Event_t *)a;
    Event_t *d = (Event_t *)b;

    if (c->uuid > d->uuid)
        return 1;
    else if (c->uuid < d->uuid)
        return -1;
    else
        return 0;
}

static int _cmpEventIndexStart(const void *a, const void *b)
{
    Event_t *c = *(Event_t **)a;
    Event_t *d = *(Event_t **)b;
    time_t t1, t2;

    if (c->start == NULL || d->start == NULL)
    {
        if (d->start != NULL)
            return 1;
        else if (c->start != NULL)
            return -1;
        else if (c->uuid > d->uuid)
            return 1;
        else if (c->uuid < d->uuid)
            return -1;
        else
            return 0;
    }

    t1 = *(c->start);
    t2 = *(d->start);

    if (t1 > t2)
        return 1;
    else if (t1 < t2)
        return -1;
    else if (c->uuid > d->uuid)
        return 1;
    else if (c->uuid < d->uuid)
        return -1;
    else
        return 0;
}

static int _cmpEventIndexEnd(const void *a, const void *b)
{
    Event_t *c = *(Event_t **)a;
    Event_t *d = *(Event_t **)b;
    time_t t1, t2;

    if (c->end == NULL || d->end == NULL)
    {
        if (d->end != NULL)
            return 1;
        else if (c->end != NULL)
            return -1;
        else if (c->uuid > d->uuid)
            return 1;
        else if (c->uuid < d->uuid)
            return -1;
        else
            return 0;
    }

    t1 = *(c->end);
    t2 = *(d->end);

    if (t1 > t2)
        return 1;
    else if (t1 < t2)
        return -1;
    else if (c->uuid > d->uuid)
        return 1;
    else if (c->uuid < d->uuid)
        return -1;
    else
        return 0;
}

static int _cmpEventIndexNotify(const void *a, const void *b)
{
    Event_t *c = *(Event_t **)a;
    Event_t *d = *(Event_t **)b;
    time_t t1, t2;

    if (c->notify == NULL || d->notify == NULL)
    {
        if (d->notify != NULL)
            return 1;
        else if (c->notify != NULL)
            return -1;
        else if (c->uuid > d->uuid)
            return 1;
        else if (c->uuid < d->uuid)
            return -1;
        else
            return 0;
    }

    t1 = *(c->notify);
    t2 = *(d->notify);

    if (t1 > t2)
        return 1;
    else if (t1 < t2)
        return -1;
    else if (c->uuid > d->uuid)
        return 1;
    else if (c->uuid < d->uuid)
        return -1;
    else
        return 0;
}
