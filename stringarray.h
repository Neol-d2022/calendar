#ifndef _STRINGARRAY_H_LOADED
#define _STRINGARRAY_H_LOADED

#include <stdio.h>

typedef struct {
    char **strings;
    size_t length;
} StringArray_t;

void padStringArray(StringArray_t *sa);
void prependStringArray(StringArray_t *sa, const char *str);
void appendStringArray(StringArray_t *sa, const char *str);
void prependStringArrayI(StringArray_t *sa, size_t index, const char *str);
void appendStringArrayI(StringArray_t *sa, size_t index, const char *str);
void printStringArray(const StringArray_t *sa, FILE *out);
void freeStringArray(StringArray_t *sa);

#endif
