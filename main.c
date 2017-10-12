#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "userui.h"
#include "eventmanager.h"

static void testEM()
{
    unsigned long long uuid[99];
    EventManager_t em;
    Event_t e;
    EventManagerSearchResultSet_t r;
    struct tm t, d, f, g;
    time_t t0, ts[3];

    EMInit(&em);

    EventInit(&e);
    e.description = (char *)"test";
    uuid[0] = EMAdd(&em, &e);
    printf("uuid = %u\n", (unsigned int)uuid[0]);
    printf("Remove = %d\n", EMRemove(&em, uuid[0]));

    time(&t0);
    t = *localtime(&t0);
    d = t;
    f = t;
    g = t;

    memset(uuid, 0, sizeof(uuid));
    for (int i = 0; i < 99; i++)
    {
        d.tm_mday += 1;
        f.tm_mday += 2;
        g.tm_mday += 3;

        ts[0] = mktime(&d);
        ts[1] = mktime(&f);
        ts[2] = mktime(&g);

        e.start = ts + 0;
        e.end = ts + 1;
        e.notify = ts + 2;

        uuid[i] = EMAdd(&em, &e);
    }

    d = t;
    f = t;
    d.tm_mday += 25;
    f.tm_mday += 75;
    ts[0] = mktime(&d);
    ts[1] = mktime(&f);

    EMSearchStart(&em, ts + 0, ts + 1, &r);
    for (unsigned long long i = 0; i < r.length; i++)
        printf("uuid = %u\n", (unsigned int)((r.results)[i]->uuid));
    printf("\n");
    EMSRS_DeInit(&r);

    EMSearchEnd(&em, ts + 0, ts + 1, &r);
    for (unsigned long long i = 0; i < r.length; i++)
        printf("uuid = %u\n", (unsigned int)((r.results)[i]->uuid));
    printf("\n");
    EMSRS_DeInit(&r);

    EMSearchNotify(&em, ts + 0, ts + 1, &r);
    for (unsigned long long i = 0; i < r.length; i++)
        printf("uuid = %u\n", (unsigned int)((r.results)[i]->uuid));
    printf("\n");
    EMSRS_DeInit(&r);

    EMDeInit(&em);
}

int main(void)
{
    testEM();
    UIEntry(stdin, stdout);

    return 0;
}
