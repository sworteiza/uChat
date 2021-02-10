#include "server.h"

char *mx_strnew(const int size)
{
    if (size < 0)
    {
        return 0;
    }
    char *a = malloc(size + 1);
    for (int i = 0; i <= size; i++)
    {
        a[i] = '\0';
    }
    return a;
}
