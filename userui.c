#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#include "calendar.h"
#include "stringarray.h"
#include "strings.h"

typedef struct
{
    FILE *in;
    FILE *out;
    char *bufRest;
    bool exitFlag;
} _UIprompt_internal_object_t;

typedef struct
{
    const char *cmd;
    const char *helpMsg;
    void (*pf_handler)(_UIprompt_internal_object_t *io);
} CommandHandler_t;

static void _UI_handler_exit(_UIprompt_internal_object_t *io);
static void _UI_handler_help(_UIprompt_internal_object_t *io);
static void _UI_handler_today(_UIprompt_internal_object_t *io);

static CommandHandler_t ch[] = {
    {"exit", "Exit the program", _UI_handler_exit},
    {"quit", "Exit the program", _UI_handler_exit},
    {"q", "Exit the program", _UI_handler_exit},
    {"help", "Display help message", _UI_handler_help},
    {"?", "Display help message", _UI_handler_help},
    {"t", "Display current calendar", _UI_handler_today},
    {"today", "Display current calendar", _UI_handler_today},
};

static const size_t cmdLength = sizeof(ch) / sizeof(ch[0]);

static int cmpCHbycmd(const void *a, const void *b)
{
    CommandHandler_t *c = (CommandHandler_t *)a;
    CommandHandler_t *d = (CommandHandler_t *)b;

    return strcmp(c->cmd, d->cmd);
}

static void _UIprompt_command_notfound(const char *cmd, FILE *out)
{
    fprintf(out, "Unknown command '%s'.\nEnter 'help' for command list.\n", cmd);
}

static int _UIprompt(FILE *in, FILE *out)
{
    char buf[1024];
    CommandHandler_t key;
    _UIprompt_internal_object_t io;
    CommandHandler_t *c;
    char *d;

    printf("> ");
    if (!fgets(buf, sizeof(buf), in))
        return 1;

    d = strpbrk(buf, " \n");
    if (d)
        *d = '\0';

    if (buf[0] == '\0')
        return 0;

    memset(&io, 0, sizeof(io));
    io.in = in;
    io.out = out;
    io.bufRest = ((d == NULL) ? (NULL) : (d + 1));
    key.cmd = (const char *)buf;
    c = (CommandHandler_t *)bsearch(&key, ch, cmdLength, sizeof(ch[0]), cmpCHbycmd);
    if (!c)
        _UIprompt_command_notfound(buf, out);
    else
        c->pf_handler(&io);

    if (io.exitFlag)
        return 1;
    return 0;
}

static void _UI_show_current_calendar(FILE *out)
{
    struct tm now;
    StringArray_t sa;
    time_t t0;

    time(&t0);
    now = *localtime(&t0);
    if (calendarToStringArray(&now, &sa, 1) == -1)
        abort();

    printStringArray(&sa, out);
    freeStringArray(&sa);
    printf("\n");
}

void UIEntry(FILE *in, FILE *out)
{
    _UI_show_current_calendar(out);
    qsort(ch, cmdLength, sizeof(ch[0]), cmpCHbycmd);
    while (_UIprompt(in, out) == 0)
        ;

    return;
}

static void _UI_handler_exit(_UIprompt_internal_object_t *io)
{
    fprintf(io->out, "Bye. See you next time.\n");
    io->exitFlag = true;
}

static void _UI_handler_help(_UIprompt_internal_object_t *io)
{
    StringArray_t sa;
    char *p;
    size_t i, l;

    sa.length = cmdLength;
    sa.strings = (char **)malloc(sizeof(sa.strings[0]) * sa.length);
    if (!sa.strings)
        abort();
    for (i = 0; i < sa.length; i += 1)
        sa.strings[i] = nstr(ch[i].cmd);
    padStringArray(&sa);
    prependStringArray(&sa, "  ");
    appendStringArray(&sa, " | ");

    for (i = 0; i < sa.length; i += 1)
        appendStringArrayI(&sa, i, ch[i].helpMsg);
    padStringArray(&sa);
    appendStringArray(&sa, "\n");

    if (sa.length > 0)
    {
        l = strlen(sa.strings[0]);
        p = (char *)malloc(l + 2);
        if (!p)
            abort();
        p[l] = '\n';
        p[l + 1] = '\0';
        memset(p, '-', l);
    }
    else
    {
        p = (char *)malloc(1);
        if (!p)
            abort();
        p[0] = '\0';
    }

    fprintf(io->out, "Here is the list of all available commands:\n%s", p);
    printStringArray(&sa, io->out);
    fprintf(io->out, "%s\n", p);

    freeStringArray(&sa);
    free(p);
}

static void _UI_handler_today(_UIprompt_internal_object_t *io)
{
    _UI_show_current_calendar(io->out);
}
