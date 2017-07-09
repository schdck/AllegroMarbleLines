#include <stdio.h>
#include <stdlib.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#include "../include/constants.h"
#include "../include/utils.h"
#include "../include/allegro_utils.h"
#include "../include/help_functions.h"

void show_help_screen(ALLEGRO_DISPLAY *display)
{
    const int AMOUNT_OF_IMAGES = 5;
    
    int current_image_index = 0, mouseX, mouseY, selected_button = -1;
    
    bool leave = false;

    char img_name[] = "help_x.png";

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_FONT *normal_font = al_load_font("../font/default_font.ttf", 24, 0);

    if(!event_queue) 
    {
        display_error(NULL, "Erro fatal", "Erro ao inicializar fila de eventos", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(EXIT_FAILURE);
    }

    if(!normal_font)
    {
        display_error(NULL, "Erro fatal", "Erro ao carregar fontes padrão da tela de ajuda", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(EXIT_FAILURE);
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    while(current_image_index < AMOUNT_OF_IMAGES && !leave)
    {
        while(!al_event_queue_is_empty(event_queue))
        {
            ALLEGRO_EVENT event;

            al_wait_for_event(event_queue, &event);

            if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                leave = true;
            }
            else if(event.type == ALLEGRO_EVENT_MOUSE_AXES)
            {
                mouseX = event.mouse.x;
                mouseY = event.mouse.y;
            }
            else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
            {
                switch(selected_button)
                {
                    case 0:
                        leave = true;
                        break;

                    case 1:
                        if(current_image_index > 0)
                        {
                            current_image_index--;
                        }
                        break;

                    case 2:
                        if(current_image_index == AMOUNT_OF_IMAGES - 1)
                        {
                            leave = true;
                        }
                        else
                        {
                            current_image_index++;
                        }
                        break;
                }
            }
        }

        int btn_size_x = 200, btn_size_y = 40;

        int btn1_x = 10, btn2_x = btn1_x + btn_size_x + 10, btn3_x = GAME_WIDTH - 210, btn_y = GAME_HEIGHT - 50;

        selected_button = -1;

        if(mouseY >= btn_y && mouseY <= btn_y + btn_size_y)
        {
            if(mouseX >= btn1_x && mouseX <= btn1_x + btn_size_x)
            {
                selected_button = 0;
            }
            else if(mouseX >= btn2_x && mouseX <= btn2_x + btn_size_x)
            {
                selected_button = 1;
            }
            else if(mouseX >= btn3_x && mouseX <= btn3_x + btn_size_x)
            {
                selected_button = 2;
            }
        }

        img_name[5] = '0' + current_image_index + 1;

        char *image_path = strjoin("../img/help/", img_name);

        al_clear_to_color(al_map_rgb(69,69,69)); 
        al_draw_textf(normal_font, al_map_rgb(0,0,0), GAME_WIDTH / 2, GAME_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "%s", image_path);
        // al_draw_bitmap(al_load_image(image_path), 0, 0, 0);

        free(image_path);

        // SAIR
        al_draw_filled_rectangle(btn1_x, btn_y, btn1_x + btn_size_x, btn_y + btn_size_y, selected_button == 0 ? al_map_rgb(204,204,204) : al_map_rgb(255,51,0));
        al_draw_text(normal_font, al_map_rgb(0,0,0), btn1_x + (btn_size_x / 2), btn_y + 10, ALLEGRO_ALIGN_CENTER, "SAIR");

        // PREVIOUS
        if(current_image_index == 0)
        {
            al_draw_filled_rectangle(btn2_x, btn_y, btn2_x + btn_size_x, btn_y + btn_size_y, al_map_rgb(100,100,100));
            al_draw_text(normal_font, al_map_rgb(0,0,0), btn2_x + (btn_size_x / 2), btn_y + 10, ALLEGRO_ALIGN_CENTER, "< VOLTAR");
        }
        else
        {
            al_draw_filled_rectangle(btn2_x, btn_y, btn2_x + btn_size_x, btn_y + btn_size_y, selected_button == 1 ? al_map_rgb(204,204,204) : al_map_rgb(150,150,150));
            al_draw_text(normal_font, al_map_rgb(0,0,0), btn2_x + (btn_size_x / 2), btn_y + 10, ALLEGRO_ALIGN_CENTER, "< VOLTAR");
        }

        // STATUS
        al_draw_filled_rectangle(btn2_x + btn_size_x + 10, btn_y, btn3_x - 10, btn_y + btn_size_y, al_map_rgb(150,150,150));
        al_draw_textf(normal_font, al_map_rgb(0,0,0), (btn2_x + btn3_x + btn_size_x) / 2, btn_y + 10, ALLEGRO_ALIGN_CENTER, "PÁGINA %d/%d", current_image_index + 1, AMOUNT_OF_IMAGES);

        // NEXT        
        if(current_image_index < AMOUNT_OF_IMAGES - 1)
        {
            al_draw_filled_rectangle(btn3_x, btn_y, btn3_x + btn_size_x, btn_y + btn_size_y, selected_button == 2 ? al_map_rgb(204,204,204) : al_map_rgb(150,150,150));
            al_draw_text(normal_font, al_map_rgb(0,0,0), btn3_x + (btn_size_x / 2), btn_y + 10, ALLEGRO_ALIGN_CENTER, "AVANÇAR >");
        }
        else
        {
            al_draw_filled_rectangle(btn3_x, btn_y, btn3_x + btn_size_x, btn_y + btn_size_y, selected_button == 2 ? al_map_rgb(204,204,204) : al_map_rgb(51,204,51));
            al_draw_text(normal_font, al_map_rgb(0,0,0), btn3_x + (btn_size_x / 2), btn_y + 10, ALLEGRO_ALIGN_CENTER, "CONCLUIR >");
        }

        al_flip_display();
    }
}