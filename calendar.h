#ifndef _CALENDER_H_LOADED
#define _CALENDER_H_LOADED

#include <time.h>
#include <stddef.h>

#include "stringarray.h"

int calendarToStringArray(const struct tm *m, StringArray_t *sa, int showCurrentDate);

#endif
