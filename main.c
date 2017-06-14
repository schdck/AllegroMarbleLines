#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>

#include "libs/utils.h"
#include "libs/allegro_utils.h"

// g++ main.c -o main -I/usr/include/allegro5 -L/usr/lib -lallegro -lallegro_image -lallegro_dialog


int main(int argc, char **argv)
{
    ALLEGRO_DISPLAY *main_display = NULL;
    ALLEGRO_BITMAP *image = NULL;

    if(!al_init()) 
    {
        fprintf(stderr, "Falha ao inicializar o allegro!\n");
        return -1;
    }

    if(!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao inicializar extensão de imagens!\n");
        return -1;
    }

    main_display = al_create_display(800, 600);
    if(!main_display) 
    {
        fprintf(stderr, "Falha ao criar o display!\n");
        return -1;
    }

    al_clear_to_color(al_map_rgb(255,255,255));

    image = load_image("img/icon.png");

    al_draw_bitmap(image, 0, 0, 0);

    al_flip_display();

    al_rest(10.0);

    al_destroy_display(main_display);
    
    al_destroy_bitmap(image);
    return 0;
}