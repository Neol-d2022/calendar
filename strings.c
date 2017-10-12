#include <stdlib.h>
#include <string.h>

char *nstr(const char *s)
{
    size_t l;
    char *n;

    l = strlen(s);
    n = (char *)malloc(l + 1);
    if (n)
    {
        memcpy(n, s, l);
        n[l] = '\0';
    }
    else
        abort();

    return n;
}
