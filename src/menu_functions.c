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
#include "../include/recorde_functions.h"
#include "../include/game_functions.h"
#include "../include/menu_functions.h"

void draw_menu_images(int highlightIndex)
{
    write_log(DEBUG_LEVEL_ALL, true, "Iniciando a função DesenharImagens.");

    int currentX = (MENU_WIDTH - 710) / 2, currentY = 225, increment = 90;

    //Desenhar fundo
    ALLEGRO_BITMAP *image = load_image(FUNDO_PATH); 
    //al_draw_bitmap(image, 0, 0, 0);
    al_destroy_bitmap(image);

    // Desenhar o logo
    image = load_image(LOGO_PATH);
    al_draw_bitmap(image, currentX, 45, 0);
    al_destroy_bitmap(image);
    
    // Até 10 imagens, caso contrário será necessário adaptar a forma em que elas são carregadas
    const int quantidadeDeImagens = 5;
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

        char *str =strjoin(imgsPath, fileName);

        image = load_image(str);

        free(str);

        al_draw_bitmap(image, currentX, currentY, 0);

        al_destroy_bitmap(image); 

        currentY += increment;
    }

    write_log(DEBUG_LEVEL_ALL, true, "Deixando a função DesenharImagens.");
}

void show_menu_screen()
{
    write_log(DEBUG_LEVEL_ALL, true, "Iniciando a função show_menu_screen.");

    ALLEGRO_DISPLAY *main_display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_FONT *default_font = NULL;

    main_display = al_create_display(MENU_WIDTH, MENU_HEIGHT);

    if(!main_display) 
    {
        display_error(NULL, "Erro fatal", "Erro ao criar o display principal", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(EXIT_FAILURE);
    }

    if(!al_set_system_mouse_cursor(main_display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        display_error(NULL, "Erro fatal", "Erro ao inicializar ponteiro do mouse", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(EXIT_FAILURE);
    }

    event_queue = al_create_event_queue();

    if(!event_queue) 
    {
        display_error(NULL, "Erro fatal", "Erro ao inicializar fila de eventos", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(EXIT_FAILURE);
    }

    if (!al_init_font_addon())
    {
        display_error(NULL, "Erro fatal", "Erro ao inicializar extensão de fontes", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(EXIT_FAILURE);
    }
 
    if (!al_init_ttf_addon())
    {
        display_error(NULL, "Erro fatal", "Erro ao inicializar extensão de fontes TTF", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(EXIT_FAILURE);
    }

    default_font = al_load_font("../font/default_font.ttf", 24, 0);

    if (!default_font)
    {
        display_error(NULL, "Erro fatal", "Erro ao carregar fonte padrão", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(main_display);
        exit(EXIT_FAILURE);
    }

    al_set_window_title(main_display, "Menu - AllegroMarbleLines");

    al_register_event_source(event_queue, al_get_display_event_source(main_display));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    bool desejaSair = false;
    int highlightIndex = 0;

    while(!desejaSair)
    {
        al_clear_to_color(al_map_rgb(0,0,0));

        draw_menu_images(highlightIndex);

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

                if(mouseX >= 45 && mouseX <= mouseX <= (GAME_WIDTH - 45))
                {
                    mouseY -= 225; // Altura do logo + margens

                    highlightIndex = (mouseY / 90) + 1;

                    if(mouseY - ((highlightIndex - 1) * 90) > 51 || mouseY < 0)
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

                        show_game_screen(x + 50, y + 50, GAME_MODE_NORMAL, 1, 20, 90);

                        al_set_target_backbuffer(main_display);

                        highlightIndex = -1; 

                        while(al_get_next_event(event_queue, &event)) { } // Esvaziar a fila de eventos

                        break;
                    case 2:
                        show_aftergame_screen(main_display, -1, -1);

                        highlightIndex = -1;

                        while(al_get_next_event(event_queue, &event)) { } // Esvaziar a fila de eventos

                        break;
                    case 3:
                        display_error(NULL, "", "Ajustes", "");
                        break;
                    case 4:
                        desejaSair = true;
                        break;
                }
            }
        }
    }

    al_destroy_display(main_display);

    write_log(DEBUG_LEVEL_ALL, true, "Deixando a função show_menu_screen.");
}