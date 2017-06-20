#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>

#include "../include/utils.h"
#include "../include/allegro_utils.h"

void DisplayError(ALLEGRO_DISPLAY *display, const char *title, const char *subtitle, const char *message)
{
    al_show_native_message_box(display, title, subtitle, message, NULL, ALLEGRO_MESSAGEBOX_ERROR);
}

struct ALLEGRO_BITMAP *load_image(const char *path)
{
    ALLEGRO_BITMAP *image = al_load_bitmap(path);

    if(image == NULL)
    {
        DisplayError(NULL, "Erro fatal", join_strings("Erro ao carregar a imagem ", path), "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(-1);
    }

    return image;
}