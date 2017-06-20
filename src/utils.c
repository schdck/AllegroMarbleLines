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