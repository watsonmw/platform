#include "stdafx.h"
#include "string.h"

int strcpyn(char *d, char *s, int n)
{
    int i = 0;
    for (; i < n; i++) {
        d[i] = s[i];

        if (s[i] == 0) {
            break;
        }
    }

    d[i] = 0;

    return i;
}

int strcpyuntil(char *d, char *s, char c)
{
    int i = 0;

    while (s[i] != c) {
        d[i] = s[i];

        if (s[i] == 0) {
            break;
        }

        i++;
    }

    d[i] = 0;

    return i;
}
