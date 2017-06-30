#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "../include/constants.h"
#include "../include/utils.h"
#include "../include/allegro_utils.h"
#include "../include/menu_functions.h"
#include "../include/game_functions.h"

void menu_DesenharImagens(int highlightIndex)
{
    int currentX = (MENU_WIDTH - 710) / 2, currentY = 225, increment = 70;

    // Desenhar o logo
    ALLEGRO_BITMAP *image = load_image(LOGO_PATH);
    al_draw_bitmap(image, currentX, 45, 0);

    // Até 10 imagens, caso contrário será necessário adaptar a forma em que elas são carregadas
    const int quantidadeDeImagens = 6;
    const char *imgsPath = "../img/menu/";

    for(int i = 1; i < quantidadeDeImagens; i++)
    {
        char fileName[12];

        if(i == highlightIndex)
        {
            strcpy(fileName, "btnx_hover.png");
        }
        else
        {
            strcpy(fileName, "btnx.png");
        }

        fileName[3] = i + '0';

        image = load_image(join_strings(imgsPath, fileName));

        al_draw_bitmap(image, currentX, currentY, 0);

        currentY += increment;
    }

    al_destroy_bitmap(image); 
}

void ExibirJanelaMenu()
{
    ALLEGRO_DISPLAY *main_display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_FONT *default_font = NULL;

    main_display = al_create_display(MENU_WIDTH, MENU_HEIGHT);

    if(!main_display) 
    {
        DisplayError(NULL, "Erro fatal", "Erro ao criar o display principal", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(-1);
    }

    if(!al_set_system_mouse_cursor(main_display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        DisplayError(NULL, "Erro fatal", "Erro ao inicializar ponteiro do mouse", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(-1);
    }

    event_queue = al_create_event_queue();

    if(!event_queue) 
    {
        DisplayError(NULL, "Erro fatal", "Erro ao inicializar fila de eventos", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(-1);
    }

    if (!al_init_font_addon())
    {
        DisplayError(NULL, "Erro fatal", "Erro ao inicializar extensão de fontes", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(-1);
    }
 
    if (!al_init_ttf_addon())
    {
        DisplayError(NULL, "Erro fatal", "Erro ao inicializar extensão de fontes TTF", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(-1);
    }

    default_font = al_load_font("../font/default_font.ttf", 24, 0);

    if (!default_font)
    {
        DisplayError(NULL, "Erro fatal", "Erro ao carregar fonte padrão", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(-1);
    }

    al_set_window_title(main_display, "Menu - AllegroMarbleLines");

    al_register_event_source(event_queue, al_get_display_event_source(main_display));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    bool desejaSair = false;
    int highlightIndex = 0;

    while(!desejaSair)
    {
        al_clear_to_color(al_map_rgb(0,0,0));

        menu_DesenharImagens(highlightIndex);

        al_flip_display();

        while(!al_event_queue_is_empty(event_queue))
        {
            ALLEGRO_EVENT event;

            al_wait_for_event(event_queue, &event);

            if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                desejaSair = true;
            }
            else if(event.type == ALLEGRO_EVENT_MOUSE_AXES)
            {
                int mouseX = event.mouse.x, mouseY = event.mouse.y;

                if(mouseX >= 45 && mouseX <= 800 - 45)
                {
                    mouseY -= 225; // Altura do logo + margens

                    float index = mouseY / 70.0;

                    highlightIndex = ceil(index);

                    if(mouseY - ((highlightIndex - 1) * 70.0) > 51)
                    {
                        highlightIndex = 0;
                    }
                }
                else
                {
                    highlightIndex = 0;
                }
            }
            else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
            {
                switch(highlightIndex)
                {
                    case 1:
                        al_clear_to_color(al_map_rgb(101, 101, 101));

                        al_draw_textf(default_font, al_map_rgb(0, 0, 0), MENU_WIDTH / 2, MENU_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "Jogando...");

                        al_flip_display();

                        int x, y;
                        al_get_window_position(main_display, &x, &y);

                        ExibirJanelaJogo(x + 50, y + 50, GAME_MODE_NORMAL, 1, 15, 20);

                        al_set_target_backbuffer(main_display); 

                        while(al_get_next_event(event_queue, &event)) { } // Esvaziar a fila de eventos

                        break;
                    case 2:
                        DisplayError(NULL, "", "Recordes", "");
                        break;
                    case 3:
                        DisplayError(NULL, "", "Ajustes", "");
                        break;
                    case 4:
                        DisplayError(NULL, "", "Créditos", "");
                        break;
                    case 5:
                        desejaSair = true;
                        break;
                }
            }
        }
    }

    al_destroy_display(main_display);
}