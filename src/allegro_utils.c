#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>

#include "../include/utils.h"
#include "../include/allegro_utils.h"

void display_error(ALLEGRO_DISPLAY *display, const char *title, const char *subtitle, const char *message)
{
    al_show_native_message_box(display, title, subtitle, message, NULL, ALLEGRO_MESSAGEBOX_ERROR);
}

bool display_yes_no_confirmation(ALLEGRO_DISPLAY *display, const char *title, const char *subtitle, const char *message)
{
    return al_show_native_message_box(display, title, subtitle, message, NULL, ALLEGRO_MESSAGEBOX_YES_NO) == 1;
}

bool colors_match(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

int inicializar_allegro5()
{
    int returnCode = 1;

    if(!al_init()) 
    {
        display_error(NULL, "Erro fatal", "Erro ao carregar o allegro5", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        returnCode = 0;
    }

    if(!al_init_image_addon())
    {
        display_error(NULL, "Erro fatal", "Erro ao carregar a extensão de imagens", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        returnCode = 0;
    }

    if(!al_install_mouse())
    {
        display_error(NULL, "Erro fatal", "Erro ao carregar ao inicializar o mouse", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        returnCode = 0;
    }

    return returnCode;
}

struct ALLEGRO_BITMAP *load_image(const char *path)
{
    ALLEGRO_BITMAP *image = al_load_bitmap(path);

    if(image == NULL)
    {
        display_error(NULL, "Erro fatal", strjoin("Erro ao carregar a imagem ", path), "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(EXIT_FAILURE);
    }

    return image;
}