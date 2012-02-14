#ifndef LOG_DOT_H
#define LOG_DOT_H

#include <stdio.h>
#include <stdarg.h>

int logOpen();
void logPrint( char *p, ... );
void logClose();
void errorReport(void);

#endif
