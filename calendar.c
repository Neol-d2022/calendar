#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "strings.h"
#include "stringarray.h"
#include "linkedlist.h"

typedef struct
{
    char **stringArray;
    size_t *index;
} calendarToStringArray_internal_object_t;

static void calendarToStringArray_internal(void *data, void *param)
{
    calendarToStringArray_internal_object_t *io = (calendarToStringArray_internal_object_t *)param;

    io->stringArray[(*(io->index))++] = (char *)data;
}

int calendarToStringArray(const struct tm *m, StringArray_t *sa, int showCurrentDate)
{
    static const char *_datePostfixes[] = {" ", " ", " ", " ", " ", " ", "\n"};
    static const char *_monthStrings[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jan", "Jun", "Aug", "Sep", "Oct", "Nov", "Dec"};
    static const char *_weekdayStrings[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    char buf[64], bufd[32];
    struct tm b, e, z;
    Linked_t list;
    LinkedNode_t **c;
    calendarToStringArray_internal_object_t io;
    size_t k, o;
    char **ret;
    unsigned int w;
    int i, j;

    memcpy(&b, m, sizeof(b));
    memcpy(&e, m, sizeof(e));
    memcpy(&z, m, sizeof(z));

    b.tm_mday = 1;
    e.tm_mon += 1;
    e.tm_mday = 0;

    if (mktime(&z) == -1)
        return 1;
    if (mktime(&b) == -1)
        return 1;
    if (mktime(&e) == -1)
        return 1;

    LinkedInit(&list);
    c = &(list.head);
    sprintf(buf, "   |          %4d %s\n", 1900 + b.tm_year, _monthStrings[b.tm_mon]);
    LinkedQuickInsert(&list, &c, nstr(buf));

    buf[0] = '\0';
    strcat(buf, "Wk | ");
    for (k = 0; k < (sizeof(_weekdayStrings) / sizeof(_weekdayStrings[0])); k += 1)
    {
        sprintf(bufd, "%s%s", _weekdayStrings[k], _datePostfixes[k]);
        strcat(buf, bufd);
    }
    LinkedQuickInsert(&list, &c, nstr(buf));

    buf[0] = '\0';
    sprintf(bufd, "%2u | ", w = (b.tm_yday / 7) + 1);
    strcat(buf, bufd);
    for (i = 0; i < b.tm_wday; i += 1)
    {
        sprintf(bufd, "   %s", _datePostfixes[i]);
        strcat(buf, bufd);
    }
    for (i = b.tm_wday, j = 1; j <= e.tm_mday; i = (i + 1) % (sizeof(_datePostfixes) / sizeof(_datePostfixes[0])), j += 1)
    {
        if (j == z.tm_mday && showCurrentDate != 0)
            sprintf(bufd, "*%2d%s", j, _datePostfixes[i]);
        else
            sprintf(bufd, "%3d%s", j, _datePostfixes[i]);
        strcat(buf, bufd);
        if (i == (sizeof(_datePostfixes) / sizeof(_datePostfixes[0])) - 1)
        {
            LinkedQuickInsert(&list, &c, nstr(buf));
            buf[0] = '\0';
            if (j + 1 <= e.tm_mday)
            {
                sprintf(bufd, "%2u | ", ++w);
                strcat(buf, bufd);
            }
        }
    }
    if (buf[0] != '\0')
    {
        strcat(buf, "\n");
        LinkedQuickInsert(&list, &c, nstr(buf));
    }

    ret = (char **)malloc(sizeof(*ret) * (list.count + 1));
    if (!ret)
        abort();
    ret[list.count] = NULL;

    o = 0;
    io.stringArray = ret;
    io.index = &o;
    LinkedIterate(&list, &io, calendarToStringArray_internal);

    sa->strings = ret;
    sa->length = list.count;

    padStringArray(sa);
    appendStringArray(sa, " | ");
    LinkedDeInit(&list);
    return 0;
}
