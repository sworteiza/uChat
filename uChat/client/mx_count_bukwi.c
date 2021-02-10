#include "../inc/client.h"

int mx_count_bukwi(const char *s, char c)
{
	int i = 0;

	while (s[i] != c && s[i])
		i++;
	return i;
}
