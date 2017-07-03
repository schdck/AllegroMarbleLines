#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>

#include "../include/utils.h"
#include "../include/allegro_utils.h"
#include "../include/menu_functions.h"

// g++ -o ../app *.c -I/usr/include/allegro5 -L/usr/lib -lallegro -lallegro_image -lallegro_dialog -lallegro_font -lallegro_ttf -lallegro_primitives

int main(int argc, char **argv)
{  
    srand(time(NULL));

    if(!InicializarAllegro())
    {
        exit(EXIT_FAILURE);
    }

    ExibirJanelaMenu();

    exit(EXIT_SUCCESS);
}