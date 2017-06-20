#include "../include/menu_functions.h"

void ExibirJanelaMenu()
{
    ALLEGRO_DISPLAY *main_display = NULL;
    ALLEGRO_BITMAP *image = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    
    if(!al_init()) 
    {
        DisplayError(NULL, "Erro fatal", "Erro ao carregar o allegro5", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(-1);
    }

    if(!al_init_image_addon())
    {
        DisplayError(NULL, "Erro fatal", "Erro ao carregar a extensão de imagens", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(-1);
    }

    main_display = al_create_display(800, 600);

    if(!main_display) 
    {
        DisplayError(NULL, "Erro fatal", "Erro ao criar o display principal", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(-1);
    }

    event_queue = al_create_event_queue();
    if(!event_queue) 
    {
        DisplayError(NULL, "Erro fatal", "Erro ao inicializar fila de eventos", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(-1);
    }

    al_set_window_title(main_display, "Menu - AllegroMarbleLines");

    al_register_event_source(event_queue, al_get_display_event_source(main_display));

    al_clear_to_color(al_map_rgb(125,125,125));

    image = load_image("../img/logo.jpg");

    al_draw_bitmap(image, 280, 50, 0);

    al_flip_display();

    while(1)
    {
        ALLEGRO_EVENT ev;
        ALLEGRO_TIMEOUT timeout;

        al_init_timeout(&timeout, 0.06);

        bool get_event = al_wait_for_event_until(event_queue, &ev, &timeout);

        if(get_event && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
        {
            break;
        }
    }

    al_destroy_display(main_display);
    
    al_destroy_bitmap(image);
}