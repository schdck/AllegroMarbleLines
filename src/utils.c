#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "../include/constants.h"
#include "../include/utils.h"

char *strjoin(const char *str1, const char *str2)
{
    char *str = (char *) malloc((strlen(str1) + strlen(str2)) * sizeof(char));

    strcpy(str, str1);
    strcat(str, str2);

    return str;
}

char *convert_int(int n)
{
    int digits = 1, copy = n;

    while(copy >= 10)
    {
        digits++;

        copy /= 10;
    }

    char *str = (char *) malloc(digits * sizeof(char));

    str[digits--] = 0;

    if(n >= 0)
    {
        while(n >= 10)
        {
            str[digits--] = (n % 10) + '0';
            n /= 10;
        }

        str[digits] = (n % 10) + '0';
    }
    else
    {
        while(digits >= 0)
        {
            str[digits--] = '-';
        }
    }

    return str;
}

void write_log(int debug_level, const char *str, ...)
{
    if(debug_level <= CURRENT_DEBUG_LEVEL && debug_level > 0)
    {
        switch(debug_level)
        {
            case DEBUG_LEVEL_INFO:
                fprintf(stderr, "[%s] ", "INFO");
                break;
            
            case DEBUG_LEVEL_WARNING:
                fprintf(stderr, "[%s] ", "WARN");
                break;

            case DEBUG_LEVEL_ERROR:
                fprintf(stderr, "[%s] ", "ERRO");
                break;
        }

        va_list arglist;
        va_start(arglist, str);
        vfprintf(stderr, str, arglist);
        va_end(arglist);
        fprintf(stderr, "\n");
    }
}