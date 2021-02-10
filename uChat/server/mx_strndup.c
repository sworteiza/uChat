#include "server.h"

char *mx_strndup(const char *str, size_t n)
{
    char *a = mx_strnew(n);
    mx_strncpy(a, str, n);
    return a;
}
