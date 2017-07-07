#include <string.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "../include/constants.h"
#include "../include/allegro_utils.h"
#include "../include/recorde_functions.h"

void exibir_tela_pos_jogo(int player_score)
{
    ALLEGRO_FONT *title_font;
    ALLEGRO_FONT *normal_font;

    title_font = al_load_font("../font/default_font.ttf", 72, 0);
    normal_font = al_load_font("../font/default_font.ttf", 16, 0);

    if (!title_font || !normal_font)
    {
        display_error(NULL, "Erro fatal", "Erro ao carregar fontes padrão da tela pós-jogo", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(EXIT_FAILURE);
    }

    al_clear_to_color(al_map_rgb(0,0,0));

    al_draw_multiline_textf(title_font, al_map_rgb(255, 255, 255), GAME_WIDTH / 2, 50, 550.0, 72.0, ALLEGRO_ALIGN_CENTER, "GAME OVER\n");
    al_draw_multiline_textf(normal_font, al_map_rgb(255, 255, 255), GAME_WIDTH / 2, 122, 300.0, 16.0, ALLEGRO_ALIGN_CENTER, "Você alcançou %d pontos\n", player_score);

    al_flip_display();

    al_rest(10);
}