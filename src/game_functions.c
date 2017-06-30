#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "../include/constants.h"
#include "../include/utils.h"
#include "../include/allegro_utils.h"
#include "../include/game_functions.h"

static ushort PROJECTILE_SPEED;
static ushort BALLS_SPEED;
static ushort GAME_MODE;
static ushort LEVEL;
static int PLAYER_SCORE;

bool Jogar(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_FONT *font);

struct PROJECTILE GenerateRandomProjectile()
{
    const int AMOUNT_OF_COLORS = 3;

    ALLEGRO_COLOR colors[AMOUNT_OF_COLORS] = {
        al_map_rgb(255, 0, 0),
        al_map_rgb(0, 255, 0),
        al_map_rgb(0, 0, 255)
    };

    PROJECTILE p;

    p.color = colors[rand() % AMOUNT_OF_COLORS];

    return p;
}

void ExibirJanelaJogo(int X, int Y, ushort game_mode, ushort level, ushort projectile_speed, ushort balls_speed)
{
    ALLEGRO_DISPLAY *game_display;
    ALLEGRO_EVENT_QUEUE *game_event_queue;
    ALLEGRO_FONT *game_font;

    game_display = al_create_display(GAME_WIDTH, GAME_HEIGHT);

    al_set_window_position(game_display, X, Y);

    if(!game_display) 
    {
        DisplayError(NULL, "Erro fatal", "Erro ao criar o display principal do jogo", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(-1);
    }

    if(!al_set_system_mouse_cursor(game_display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        DisplayError(NULL, "Erro fatal", "Erro ao inicializar ponteiro do mouse", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(game_display);
        exit(-1);
    }

    game_event_queue = al_create_event_queue();

    if(!game_event_queue) 
    {
        DisplayError(NULL, "Erro fatal", "Erro ao inicializar fila de eventos", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(game_display);
        exit(-1);
    }

    if(!al_init_primitives_addon())
    {
        DisplayError(NULL, "Erro fatal", "Erro ao inicializar extensão de primitivas gráficas", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(game_display);
        exit(-1);
    }

    game_font = al_load_font("../font/default_font.ttf", 12, 0);

    if (!game_font)
    {
        DisplayError(NULL, "Erro fatal", "Erro ao carregar fonte padrão do jogo", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(game_display);
        exit(-1);
    }

    al_set_window_title(game_display, "Jogo - AllegroMarbleLines");

    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    al_register_event_source(game_event_queue, al_get_mouse_event_source());

    PROJECTILE_SPEED = projectile_speed;
    BALLS_SPEED = balls_speed;
    LEVEL = level;
    GAME_MODE = game_mode;

    PLAYER_SCORE = 0;

    while(Jogar(game_display, game_event_queue, game_font))
    {
        // XXX Exibir informações (posição no ranking, pontos, etc.)
    }

    al_destroy_font(game_font);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);
}

bool Jogar(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_FONT *font)
{
    bool leave_game = false, fired_ball = false, ball_on_way = false;

    int mouseX = 0, mouseY = 0;
    
    double speedX, speedY;

    double fps_start, fps_difference;

    PROJECTILE current_projectile = GenerateRandomProjectile(),
               next_projectile = GenerateRandomProjectile(),
               on_way_projectile;

    while(!leave_game)
    {
        fps_start = al_get_time();
        // Início laço de atualização da tela

        while(!al_event_queue_is_empty(event_queue))
        {
            ALLEGRO_EVENT event;

            al_wait_for_event(event_queue, &event);

            if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                leave_game = DisplayYesNoConfirmation(NULL, "Tem certeza?", "Você tem certeza que deseja sair?", "Qualquer progresso não salvo será perdido");
            }
            else if(event.type == ALLEGRO_EVENT_MOUSE_AXES)
            {
                mouseX = event.mouse.x;
                mouseY = event.mouse.y;
            }
            else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && !fired_ball && !ball_on_way)
            {
                fired_ball = true;

                double deltaX = mouseX - (GAME_WIDTH / 2);
                double deltaY = mouseY - (GAME_HEIGHT / 2);

                double rotation_angle = atan2(deltaX, deltaY) * -1 + M_PI;

                speedX = sin(rotation_angle) * PROJECTILE_SPEED;
                speedY = cos(rotation_angle) * -PROJECTILE_SPEED;
            }
        }

        ALLEGRO_BITMAP *background;

        switch(LEVEL)
        {
            case 1:
                background = load_image("../img/game/level_1.png");
                break;
            case 2:
                background = load_image("../img/game/level_2.png");
                break;
            case 3:
                background = load_image("../img/game/level_3.png");
                break;
            case 4:
                background = load_image("../img/game/level_4.png");
                break;
        }

        ALLEGRO_BITMAP *cannon_bg = load_image("../img/game/cannon_bg.png");
        ALLEGRO_BITMAP *cannon = load_image("../img/game/cannon.png");

        double rotation_angle = atan2f(mouseX - (GAME_WIDTH / 2), mouseY - (GAME_HEIGHT / 2)) * -1 + M_PI;

        al_clear_to_color(al_map_rgb(101,101,101));
        al_draw_bitmap(background, 0, 0, 0);

        al_draw_line(GAME_WIDTH / 2, GAME_HEIGHT / 2, mouseX, mouseY, al_map_rgb(255,0,0), 3.0);
        al_draw_bitmap(cannon_bg, GAME_WIDTH / 2 - 53, GAME_HEIGHT / 2 - 53, 0);
        al_draw_rotated_bitmap(cannon, 53, 53, GAME_WIDTH / 2, GAME_HEIGHT / 2, rotation_angle - M_PI, 0);
        al_draw_multiline_textf(font, al_map_rgb(0, 0, 0), mouseX + 15, mouseY + 15, 300.0, 12.0, ALLEGRO_ALIGN_LEFT, "angle %.2frad (%.2fdeg)\nsin %.2f\ncos %.2f", rotation_angle, rotation_angle * 57.2958, sin(rotation_angle), cos(rotation_angle));
        al_draw_multiline_textf(font, al_map_rgb(0, 0, 0), 5, 5, 300.0, 12.0, ALLEGRO_ALIGN_LEFT, "NÍVEL: %d\nPONTOS: %d", LEVEL, PLAYER_SCORE);

        if(fired_ball)
        {
            on_way_projectile = current_projectile;
            current_projectile = next_projectile;
            next_projectile = GenerateRandomProjectile();
            ball_on_way = true;
            fired_ball = false;
        }

        current_projectile.x = (GAME_WIDTH / 2) + (sin(rotation_angle) * 55);
        current_projectile.y = (GAME_HEIGHT / 2) + (cos(rotation_angle) * -55);

        next_projectile.x = (GAME_WIDTH / 2) + (sin(rotation_angle) * -35);
        next_projectile.y = (GAME_HEIGHT / 2) + (cos(rotation_angle) * 35);

        al_draw_filled_circle(current_projectile.x, current_projectile.y, 13, current_projectile.color);
        al_draw_filled_circle(next_projectile.x, next_projectile.y, 7, next_projectile.color);

        if(ball_on_way)
        {
            if(on_way_projectile.x > GAME_WIDTH || on_way_projectile.y > GAME_HEIGHT || on_way_projectile.x < 0 || on_way_projectile.y < 0)
            {
                PLAYER_SCORE -= 7;
                ball_on_way = false;
            }
            else if(0) // XXX detectar colisão
            {

            }
            else
            {
                on_way_projectile.x += speedX;
                on_way_projectile.y += speedY;

                al_draw_filled_circle(on_way_projectile.x, on_way_projectile.y, 13, on_way_projectile.color);
            }
        }

        al_flip_display();

        al_destroy_bitmap(background);
        al_destroy_bitmap(cannon_bg);
        al_destroy_bitmap(cannon);

        // Final laço de atualização da tela
        fps_difference = al_get_time() - fps_start;

        if(fps_difference < 1.0 / GAME_FPS)
        {
            al_rest((1.0 / GAME_FPS) - fps_difference);
        }
    }
}