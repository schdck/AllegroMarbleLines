#include <stdlib.h>
#include <string.h>

#include "../include/utils.h"

char *join_strings(const char *str1, const char *str2)
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