#pragma once

#include <signal.h>
#include <netdb.h>
#include <pthread.h>
#include "proto.h"
#include "string.h"
#include "gtk/gtk.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>

int mx_count_bukwi(const char *s, char c);
int mx_count_words(const char *str, char c);
char *mx_strndup(const char *str, size_t n);
char **mx_strsplit(const char *s, char c);
char *mx_strncpy(char *dst, const char *src, int len);
char *mx_strnew(const int size);
