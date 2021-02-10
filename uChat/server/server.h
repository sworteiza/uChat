#pragma once

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sqlite3.h>
#include <time.h>
#include <malloc/malloc.h>
#include <signal.h>

typedef struct s_db
{
    char *name;
    char *pass;
    char *repeatpass;
    char *login;
    int check;
    int count;
} t_db;
typedef struct s_msg
{
    char *name_who;
    char *name_whom;
    char *text_msg;
    char *time;
    int index;
} t_msg;
typedef struct s_del
{
    char *login;
    char *who;
    int iter;
    int index;
    int flag;
    sqlite3 *db;
} t_del;

int mx_count_bukwi(const char *s, char c);
int mx_count_words(const char *str, char c);
char *mx_strndup(const char *str, size_t n);
char **mx_strsplit(const char *s, char c);
char *mx_strncpy(char *dst, const char *src, int len);
char *mx_strnew(const int size);
void mx_swap_char(char *s1, char *s2);
char *mx_strcpy(char *dst, const char *src);
void mx_str_reverse(char *s);
char *mx_itoa(int number);
