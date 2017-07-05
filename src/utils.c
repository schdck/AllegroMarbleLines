#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <math.h>

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

double distance_between_points(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void write_log(int debug_level, bool print_prefix, const char *str, ...)
{
    if(debug_level <= CURRENT_DEBUG_LEVEL && debug_level > 0)
    {
        if(print_prefix)
        {
            switch(debug_level)
            {
                case DEBUG_LEVEL_ALL:
                    fprintf(stderr, "[%s] ", "ALL*");
                    break;

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
        }

        va_list arglist;
        va_start(arglist, str);
        vfprintf(stderr, str, arglist);
        va_end(arglist);
        fprintf(stderr, "\n");
    }
}