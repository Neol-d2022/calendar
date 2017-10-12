#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "strings.h"
#include "stringarray.h"

static void _insertBeforeNewline(char *buf, const char *str)
{
    char *r, *s;

    r = strchr(buf, '\n');
    if (r)
    {
        s = nstr(r);
        if (!s)
            abort();
        *r = '\0';
        strcat(buf, str);
        strcat(buf, s);
        free(s);
    }
    else
    {
        strcat(buf, str);
    }
}

void padStringArray(StringArray_t *sa)
{
    char *p, *q;
    size_t i, n, m;

    m = 0;
    for (i = 0; i < sa->length; i += 1)
    {
        n = strlen(sa->strings[i]);
        if (n > m)
            m = n;
    }

    for (i = 0; i < sa->length; i += 1)
    {
        n = strlen(sa->strings[i]);
        if (n != m)
        {
            p = malloc(m - n + 1);
            p[m - n] = '\0';
            memset(p, ' ', m - n);
            q = (char *)realloc(sa->strings[i], m + 1);
            if (!q)
                abort();
            _insertBeforeNewline(q, p);
            free(p);
            sa->strings[i] = q;
        }
    }
}

void prependStringArray(StringArray_t *sa, const char *str)
{
    char *p;
    size_t i, l, m;

    l = strlen(str);
    if (l == 0)
        return;

    for (i = 0; i < sa->length; i += 1)
    {
        m = strlen(sa->strings[i]);
        p = (char *)malloc(l + m + 1);
        if (!p)
            abort();
        memcpy(p, str, l);
        memcpy(p + l, sa->strings[i], m);
        p[l + m] = '\0';
        free(sa->strings[i]);
        sa->strings[i] = p;
    }
}

void appendStringArray(StringArray_t *sa, const char *str)
{
    char *p;
    size_t i, l, m;

    l = strlen(str);
    if (l == 0)
        return;

    for (i = 0; i < sa->length; i += 1)
    {
        m = strlen(sa->strings[i]);
        p = (char *)realloc(sa->strings[i], l + m + 1);
        if (!p)
            abort();
        _insertBeforeNewline(p, str);
        sa->strings[i] = p;
    }
}

void prependStringArrayI(StringArray_t *sa, size_t index, const char *str)
{
    char *p;
    size_t i, l, m;

    l = strlen(str);
    if (l == 0)
        return;

    i = index;
    {
        m = strlen(sa->strings[i]);
        p = (char *)malloc(l + m + 1);
        if (!p)
            abort();
        memcpy(p, str, l);
        memcpy(p + l, sa->strings[i], m);
        p[l + m] = '\0';
        free(sa->strings[i]);
        sa->strings[i] = p;
    }
}

void appendStringArrayI(StringArray_t *sa, size_t index, const char *str)
{
    char *p;
    size_t i, l, m;

    l = strlen(str);
    if (l == 0)
        return;

    i = index;
    {
        m = strlen(sa->strings[i]);
        p = (char *)realloc(sa->strings[i], l + m + 1);
        if (!p)
            abort();
        _insertBeforeNewline(p, str);
        sa->strings[i] = p;
    }
}

void printStringArray(const StringArray_t *sa, FILE *out)
{
    size_t i;

    for (i = 0; i < sa->length; i += 1)
        fprintf(out, "%s", sa->strings[i]);
}

void freeStringArray(StringArray_t *sa)
{
    size_t i;

    for (i = 0; i < sa->length; i += 1)
        free(sa->strings[i]);
    free(sa->strings);
}
