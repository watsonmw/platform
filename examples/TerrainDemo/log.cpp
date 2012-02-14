#include "stdafx.h"

#include "log.h"
#include <stdlib.h>

FILE *logfileptr;

int logOpen()
{
    logfileptr = fopen("log.txt", "w");

    if (logfileptr == NULL) {
        return 0;
    }

    fclose(logfileptr);
    return 1;
}

void logClose()
{
    if (logfileptr != NULL) {
        fclose(logfileptr);
    }
}

void logPrint( char *p, ... )
{
    char    buffer[65555];
    va_list args;

    va_start(args, p);
    vsprintf(buffer, p, args);
    va_end(args);

    logfileptr = fopen("log.txt", "a");
    if (logfileptr != NULL) {
        fprintf(logfileptr, "%s\n", buffer);
        fclose(logfileptr);
    }
}

void errorReport(void)
{
    int r = glGetError();

    logPrint("OGL Report: %s", gluErrorString(r));
}
