#include <stdio.h>
#include <string.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#include "../include/constants.h"
#include "../include/utils.h"
#include "../include/allegro_utils.h"
#include "../include/recorde_functions.h"

struct RECORDE {
    char nome[24];
    int pontos;
    int level;
};

int obter_posicao_encaixe(RECORDE r[10], RECORDE *search)
{
    for(int i = 0; i < 10; i++)
    {
        if(r[i].pontos < search->pontos)
        {
            return i;
        }
        else if(r[i].pontos == search->pontos && r[i].level < search->level)
        {
            return i;
        }
    }

    return -1;
}

RECORDE *criar_recorde(const char nome[], int pontos, int level)
{
    RECORDE *r = (RECORDE *) malloc(sizeof(RECORDE));

    strcpy(r->nome, nome);

    r->pontos = pontos;
    r->level = level;

    return r;
}

void write_list_to_file(const char file_nome[], RECORDE *array)
{
    FILE *file = fopen(file_nome, "w");

    if(!file)
    {
        display_error(NULL, "Erro fatal", "Erro ao abrir o arquivo de recordes", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 10; i++)
    {
        fprintf(file, "%d,%d,%s\n", array[i].pontos, array[i].level, array[i].nome);
    }

    fclose(file);

}

RECORDE *read_list_from_file(const char file_nome[])
{
    RECORDE *recordes = (RECORDE *) malloc(10 * sizeof(RECORDE));

    FILE *file = fopen(file_nome, "r");

    if(!file)
    {
        file = fopen(file_nome, "w");

        if(!file)
        {
            display_error(NULL, "Erro fatal", "Erro ao criar o arquivo de recordes", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < 10; i++)
        {
            RECORDE *destination = (recordes + i);

            strcpy(destination->nome, "-");

            destination->level = 0;
            destination->pontos = 0;

            fprintf(file, "%d,%d,-\n", 0, 0);
        }

        fclose(file);

        return recordes;
    }

    int c = 0, pontos;

    while(c < 10 && fscanf(file, "%d,%d,%s", &(recordes + c)->pontos, &(recordes + c)->level, (recordes + c)->nome) == 3)
    {
        c++;
    }

    return recordes;
}

void exibir_tela_pos_jogo(ALLEGRO_DISPLAY *display, int player_score, int player_level)
{
    ALLEGRO_FONT *title_font = al_load_font("../font/default_font.ttf", 72, 0);
    ALLEGRO_FONT *normal_font = al_load_font("../font/default_font.ttf", 24, 0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();

    if(!event_queue) 
    {
        display_error(NULL, "Erro fatal", "Erro ao inicializar fila de eventos", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(EXIT_FAILURE);
    }

    if (!title_font || !normal_font)
    {
        display_error(NULL, "Erro fatal", "Erro ao carregar fontes padrão da tela pós-jogo", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(EXIT_FAILURE);
    }

    RECORDE *recordes = read_list_from_file("../data/highscores.csv");

    RECORDE *current = criar_recorde("", player_score, player_level);

    int index_with_score = obter_posicao_encaixe(recordes, current); 

    char alfabeto[] = "abcdefghijklmnopqrstuvwxyz";

    int contador_pointer, selected_button = -1, mouseX, mouseY;

    bool leave = false, show_pointer = true;

    double fps_start, fps_difference;

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    while(!leave)
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
            else if(event.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                int keycode = event.keyboard.keycode;
                int len = strlen(current->nome);

                if(keycode == ALLEGRO_KEY_BACKSPACE)
                {
                    if(len > 0)
                    {
                        current->nome[len - 1] = 0;
                    }
                }
                else if(len < 24)
                {
                    if(keycode >= ALLEGRO_KEY_A && keycode <= ALLEGRO_KEY_Z)
                    {
                        current->nome[len] = alfabeto[keycode - 1];
                        current->nome[len + 1] = 0;
                    }
                    else if(keycode == ALLEGRO_KEY_MINUS)
                    {
                        current->nome[len] = '-';
                        current->nome[len + 1] = 0;
                    }
                }
            }
            else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            {
                leave = selected_button >= 0;
            }
        }

        if(--contador_pointer <= 0)
        {
            show_pointer = !show_pointer;

            contador_pointer = GAME_FPS /2;
        }

        fps_start = al_get_time();

        al_clear_to_color(al_map_rgb(69,69,69));   

        al_draw_text(title_font, al_map_rgb(255, 255, 255), GAME_WIDTH / 2, 40, ALLEGRO_ALIGN_CENTER, "PLACAR DE LÍDERES");

        if(player_score > 0)
        {
            al_draw_textf(normal_font, al_map_rgb(255, 255, 255), GAME_WIDTH /2, 100, ALLEGRO_ALIGN_CENTER, "Você fez %d pontos", player_score);
        }

        int x1 = 45, x2 = GAME_WIDTH - 45, y = 130;

        al_draw_rectangle(x1, y, x2, y + 305, al_map_rgb(255,255,255), 3.0);

        y += 15;

        al_draw_text(normal_font, al_map_rgb(200, 200, 200), x1 + 020, y, ALLEGRO_ALIGN_LEFT, "NOME");
        al_draw_text(normal_font, al_map_rgb(200, 200, 200), x2 - 170, y, ALLEGRO_ALIGN_CENTER, "PONTOS");
        al_draw_text(normal_font, al_map_rgb(200, 200, 200), x2 - 070, y, ALLEGRO_ALIGN_CENTER, "NÍVEL");

        y += 25;

        for(int i = 0; i < 10; i++, y += 25)
        {
            al_draw_textf(normal_font, al_map_rgb(255, 255, 255), x1 + 020, y, ALLEGRO_ALIGN_LEFT, "%s", recordes[i].nome);
            al_draw_textf(normal_font, al_map_rgb(255, 255, 255), x2 - 170, y, ALLEGRO_ALIGN_CENTER, "%d", recordes[i].pontos);
            al_draw_textf(normal_font, al_map_rgb(255, 255, 255), x2 - 070, y, ALLEGRO_ALIGN_CENTER, "%d", recordes[i].level);
        }

        if(index_with_score >= 0)
        {
            if(mouseX >= 45 && mouseX <= MENU_WIDTH / 2 - 5 && mouseY >= 510 && mouseY <= 560)
            {
                selected_button = 0;
            }
            else if(mouseX >= MENU_WIDTH / 2 + 5 && mouseX <= MENU_WIDTH - 45 && mouseY >= 510 && mouseY <= 560)
            {
                selected_button = 1;
            }
            else
            {
                selected_button = -1;
            }

            al_draw_filled_rectangle(45, 445, MENU_WIDTH - 45, 500, al_map_rgb(255,255,255));

            if(strlen(current->nome) == 0)
            {
                al_draw_textf(normal_font, al_map_rgb(69,69,69), 50, 460, ALLEGRO_ALIGN_LEFT, " Insira seu nome (apenas letras e traços - max: 24)");

                if(show_pointer)
                {
                    al_draw_textf(normal_font, al_map_rgb(0,0,0), 50, 460, ALLEGRO_ALIGN_LEFT, "|");
                }
            }
            else
            {
                al_draw_textf(normal_font, al_map_rgb(0,0,0), 50, 460, ALLEGRO_ALIGN_LEFT, " %s%s", current->nome, show_pointer ? "|" : "");
            }

            al_draw_filled_rectangle(45, 510, MENU_WIDTH / 2 - 5, 560, selected_button == 0 ? al_map_rgb(204,204,204) : al_map_rgb(255,51,0));
            al_draw_filled_rectangle(MENU_WIDTH / 2 + 5, 510, MENU_WIDTH - 45, 560, selected_button == 1 ? al_map_rgb(204,204,204) : al_map_rgb(51,204,51));

            al_draw_text(normal_font, al_map_rgb(0,0,0), (MENU_WIDTH / 2 - 5 + 45) / 2, 525 , ALLEGRO_ALIGN_CENTER, "SAIR");
            al_draw_text(normal_font, al_map_rgb(0,0,0), (MENU_WIDTH / 2 + 5 + MENU_WIDTH - 45) / 2, 525, ALLEGRO_ALIGN_CENTER, "SALVAR");
        }
        else
        {
            if(mouseX >= 45 && mouseX <= MENU_WIDTH - 45 && mouseY >= 510 && mouseY <= 560)
            {
                selected_button = 0;
            }
            else
            {
                selected_button = -1;
            }

            al_draw_filled_rectangle(45, 510, MENU_WIDTH - 45, 560, selected_button == 0 ? al_map_rgb(204,204,204) : al_map_rgb(255,51,0));

            al_draw_text(normal_font, al_map_rgb(0,0,0), MENU_WIDTH / 2, 525 , ALLEGRO_ALIGN_CENTER, "SAIR");
        }

        al_flip_display();

        fps_difference = al_get_time() - fps_start;

        if(fps_difference < 1.0 / GAME_FPS)
        {
            al_rest((1.0 / GAME_FPS) - fps_difference);
        }
    }

    if(selected_button == 1)
    {
        for(int i = 9; i > index_with_score; i--)
        {
            recordes[i] = recordes[i - 1];
        }

        strcpy(recordes[index_with_score].nome, current->nome);
        recordes[index_with_score].pontos = current->pontos;
        recordes[index_with_score].level = current->level;

        write_list_to_file("../data/highscores.csv", recordes);
    }

    al_destroy_font(title_font);
    al_destroy_font(normal_font);
    al_destroy_event_queue(event_queue);
}