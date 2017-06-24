#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>

#include "../include/utils.h"
#include "../include/allegro_utils.h"
#include "../include/menu_functions.h"

// g++ -o ../app allegro_utils.c menu_functions.c main.c utils.c -I/usr/include/allegro5 -L/usr/lib -lallegro -lallegro_image -lallegro_dialog

int main(int argc, char **argv)
{  
    if(!InicializarAllegro())
    {
        exit(-1);
    }

    ExibirJanelaMenu();

    return 0;
}