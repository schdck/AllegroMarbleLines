#include <stdio.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "../include/constants.h"
#include "../include/utils.h"
#include "../include/allegro_utils.h"
#include "../include/recorde_functions.h"
#include "../include/game_functions.h"

#define AMOUNT_OF_COLORS    6

ALLEGRO_COLOR COLORS[AMOUNT_OF_COLORS];

int GENERATED_BALLS_OF_COLOR[AMOUNT_OF_COLORS];
int BALLS_TO_COME;

static ushort PROJECTILE_SPEED;
static ushort BALLS_SPEED;
static ushort GAME_MODE;
static ushort LEVEL;
static int PLAYER_SCORE;

int play(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_FONT *font);

struct ALLEGRO_COLOR generate_random_color(bool is_for_projectile)
{
    int index;

    do
    {
        index = rand() % (4 + ((LEVEL - 1) / 3));
    } while(GENERATED_BALLS_OF_COLOR[index] == 0 && is_for_projectile);

    if(!is_for_projectile)
    {
        GENERATED_BALLS_OF_COLOR[index]++;
    }

    return COLORS[index];
}

struct BALL generate_ball()
{
    BALL b;

    b.position = 0;
    b.color = generate_random_color(false);

    return b;
}

void initialize_generated_balls_at_color_array()
{
    for(int i = 0; i < AMOUNT_OF_COLORS; i++)
    {
        GENERATED_BALLS_OF_COLOR[i] = 0;
    }
}

void shift_array(BALL *a, int n, int d) 
{
   for (int i = n; i >= 0 + d; i--)
   {
       *(a + i) = *(a + i - d);
       *(a + i) = *(a + i - d);
   }
}

void prepare_for_level(int level, int delay_in_seconds, ALLEGRO_EVENT_QUEUE *queue)
{
    ALLEGRO_FONT *title_font = al_load_font("../font/default_font.ttf", 56, 0),
                 *subtitle_font = al_load_font("../font/default_font.ttf", 16, 0);

    while(delay_in_seconds-- > 0)
    {
        al_clear_to_color(al_map_rgb(0,0,0));

        al_draw_textf(title_font, al_map_rgb(255,255,255), GAME_WIDTH / 2, GAME_HEIGHT / 2 - 61, ALLEGRO_ALIGN_CENTER, "LEVEL %d", LEVEL);
        
        al_draw_textf(subtitle_font, al_map_rgb(255,255,255), GAME_WIDTH / 2, GAME_HEIGHT / 2 - 7, ALLEGRO_ALIGN_CENTER, "Começando em %d segundos", delay_in_seconds + 1);

        al_flip_display();        

        al_rest(1);
    }

    ALLEGRO_EVENT event;

    while(al_get_next_event(queue, &event)) { } // Esvaziar a fila de eventos
}

void show_game_screen(int X, int Y, ushort game_mode, ushort level, ushort start_amout_of_balls, ushort balls_speed)
{
    write_log(DEBUG_LEVEL_ALL, true, "Iniciando a função show_game_screen.");

    COLORS[0] = al_map_rgb(255, 143, 143);
    COLORS[1] = al_map_rgb(143, 255, 143);
    COLORS[2] = al_map_rgb(143, 143, 255);
    COLORS[3] = al_map_rgb(255, 255, 123);
    COLORS[4] = al_map_rgb(255, 123, 255);
    COLORS[5] = al_map_rgb(123, 255, 255);

    ALLEGRO_DISPLAY *game_display;
    ALLEGRO_EVENT_QUEUE *game_event_queue;
    ALLEGRO_FONT *game_font;

    game_display = al_create_display(GAME_WIDTH, GAME_HEIGHT);

    al_set_window_position(game_display, X, Y);

    if(!game_display) 
    {
        display_error(NULL, "Erro fatal", "Erro ao criar o display principal do jogo", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        exit(EXIT_FAILURE);
    }

    if(!al_set_system_mouse_cursor(game_display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        display_error(NULL, "Erro fatal", "Erro ao inicializar ponteiro do mouse", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(game_display);
        exit(EXIT_FAILURE);
    }

    game_event_queue = al_create_event_queue();

    if(!game_event_queue) 
    {
        display_error(NULL, "Erro fatal", "Erro ao inicializar fila de eventos", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(game_display);
        exit(EXIT_FAILURE);
    }

    game_font = al_load_font("../font/default_font.ttf", 20, 0);

    if (!game_font)
    {
        display_error(NULL, "Erro fatal", "Erro ao carregar fonte padrão do jogo", "Esperamos resolver isso em breve. O aplicativo será encerrado.");
        al_destroy_display(game_display);
        exit(EXIT_FAILURE);
    }

    al_set_window_title(game_display, "Jogo - AllegroMarbleLines");

    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    al_register_event_source(game_event_queue, al_get_mouse_event_source());

    PROJECTILE_SPEED = 15;
    LEVEL = level - 1;
    BALLS_SPEED = balls_speed + ((balls_speed / 4) * (LEVEL / 4));
    GAME_MODE = game_mode;
    PLAYER_SCORE = 0;

    int game_return_code;

    do
    {
        BALLS_SPEED = balls_speed + ((balls_speed / 4) * (LEVEL / 4));
        BALLS_TO_COME = start_amout_of_balls + (5 * LEVEL);
        LEVEL++;

        prepare_for_level(LEVEL, 3, game_event_queue);

        game_return_code = play(game_display, game_event_queue, game_font);

    } while(game_return_code == GAME_RETURN_CODE_WON);

    if(game_return_code == GAME_RETURN_CODE_LOST)
    {
        al_destroy_font(game_font);

        game_font = al_load_font("../font/default_font.ttf", 48, 0);

        al_clear_to_color(al_map_rgb(0,0,0));

        al_draw_text(game_font, al_map_rgb(255,0,0), GAME_WIDTH / 2, GAME_HEIGHT / 2 - 40, ALLEGRO_ALIGN_CENTER, "GAME OVER");

        al_flip_display();

        al_rest(2.0);

        show_aftergame_screen(game_display, PLAYER_SCORE, LEVEL);
    }

    al_destroy_font(game_font);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);

    write_log(DEBUG_LEVEL_ALL, true, "Deixando a função show_game_screen.");
}

void load_level(struct ALLEGRO_BITMAP **level_background, struct MAP_INFO **map_info)
{
    write_log(DEBUG_LEVEL_ALL, true, "Iniciando a função load_level.");

    char file_name[] = "level_x.csv";

    switch(LEVEL % 3)
    {
        case 1:
            *level_background = load_image("../img/game/level_1.png");
            file_name[6] = 1 + '0';
            break;
        case 2:
            *level_background = load_image("../img/game/level_2.png");
            file_name[6] = 2 + '0';
            break;
        case 0:
            *level_background = load_image("../img/game/level_3.png");
            file_name[6] = 3 + '0';
            break;
    }

    FILE *level_info;
    char *line = NULL;
    size_t line_length;

    level_info = fopen(strjoin("../data/", file_name), "r");

    int points_in_track = 0, amount_of_tracks = 0, track_lenght[GAME_MAX_TRACKS];

    for(int i = 0; i < GAME_MAX_TRACKS; i++)
    {
        track_lenght[i] = 0;
    }

    while (getline(&line, &line_length, level_info) != -1)
    {
        points_in_track++;

        if(line[0] == '-')
        {
            track_lenght[amount_of_tracks] = points_in_track - 1;

            points_in_track = 0;
            amount_of_tracks++;
        }
    }

    track_lenght[amount_of_tracks] = points_in_track;
    (*map_info) = (MAP_INFO *) malloc(sizeof(MAP_INFO));
    (*map_info)->map_length = amount_of_tracks;
    
    (*map_info)->tracks = (TRACK *) malloc(amount_of_tracks * sizeof(TRACK));    

    write_log(DEBUG_LEVEL_INFO, false, "***** LEVEL [INFO] *****\nFile name: %s\nAmount of tracks: %d\n", file_name, amount_of_tracks);

    for(int i = 0; i < amount_of_tracks; i++)
    {
        (*map_info)->tracks[i].track_length = track_lenght[i];
        
        (*map_info)->tracks[i].path = (POINT *) malloc(sizeof(POINT) * track_lenght[i]);
    }

    // Voltar ao início do arquivo
    rewind(level_info);

    int position = 0, track = 0, point_x, point_y, vulnerable;

    POINT *p = &(*map_info)->tracks[0].path[0];

    while(fscanf(level_info, "%d,%d,%d", &point_x, &point_y, &vulnerable) == 3)
    {
        if(point_x >= 0 && point_y >= 0)
        {
            p = &(*map_info)->tracks[track].path[position++];

            p->ball_vulnerable = vulnerable;
            p->x = point_x;
            p->y = point_y;
        }
        else
        {
            write_log(DEBUG_LEVEL_INFO, true, "Leitura da track %d concluída (%d pontos).", track, (*map_info)->tracks[track].track_length);

            track++;
            position = 0;
        }
    }
    
    fclose(level_info);

    write_log(DEBUG_LEVEL_ALL, true, "Deixando a função load_level.");
}

void reset_screen(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *cannon_bg, ALLEGRO_BITMAP *cannon, double rotation_angle, ALLEGRO_FONT *font)
{
    al_clear_to_color(al_map_rgb(101,101,101));
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_bitmap(cannon_bg, GAME_WIDTH / 2 - 53, GAME_HEIGHT / 2 - 53, 0);
    al_draw_rotated_bitmap(cannon, 53, 53, GAME_WIDTH / 2, GAME_HEIGHT / 2, rotation_angle - M_PI, 0);

    //al_draw_multiline_textf(font, al_map_rgb(0, 0, 0), mouseX + 15, mouseY + 15, 300.0, 12.0, ALLEGRO_ALIGN_LEFT, "angle %.2frad (%.2fdeg)\nsin %.2f\ncos %.2f", rotation_angle, rotation_angle * 57.2958, sin(rotation_angle), cos(rotation_angle));
    al_draw_multiline_textf(font, al_map_rgb(0, 0, 0), GAME_WIDTH - 5, 5, 500.0, 18.0, ALLEGRO_ALIGN_RIGHT, "NÍVEL: %d\nPONTOS: %d", LEVEL, PLAYER_SCORE);
}

int advance_balls(MAP_INFO *map_info, BALL *ball_at_track[], int *created_balls_at_track, int extra_deslocamento)
{
    for(int i = 0; i < map_info->map_length; i++)
    {
        for(int j = 0; j < created_balls_at_track[i]; j++)
        {
            BALL *b = &ball_at_track[i][j];

            b->position += (1 + extra_deslocamento);

            if(b->position >= map_info->tracks[i].track_length)
            {
                b->position = map_info->tracks[i].track_length - 1;

                created_balls_at_track[i]--;

                return i;
            }
        }
    }

    return -1;
}

bool create_ball_at_track(int track, MAP_INFO *map_info, BALL *ball_at_track[], int *created_balls_at_track)
{
    if(created_balls_at_track[track] > 0)
    {
        shift_array(ball_at_track[track], created_balls_at_track[track], 1);
    }

    ball_at_track[track][0] = generate_ball();

    BALLS_TO_COME--;

    created_balls_at_track[track]++;
}

void draw_balls(MAP_INFO *map_info, BALL *ball_at_track[], int *created_balls_at_track)
{
    for(int i = 0; i < map_info->map_length; i++)
    {
        for(int j = 0; j < created_balls_at_track[i]; j++)
        {
            BALL b = ball_at_track[i][j];

            POINT p = map_info->tracks[i].path[b.position];

            if(p.ball_vulnerable > 0)
            {
                al_draw_filled_circle(p.x, p.y, p.ball_vulnerable == 1 ? GAME_PROJECTILE_RADIUS : GAME_NEXT_PROJECTILE_RADIUS, b.color);
                //al_draw_text(font, al_map_rgb(0, 0, 0), p.x, p.y, 0, convert_int(p.ball_vulnerable));
            }
            
        }
    } 
}

int play(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_FONT *font)
{
    write_log(DEBUG_LEVEL_ALL, true, "Iniciando a função play.");

    bool fired_projectile = false, projectile_on_way = false, hold_positions = false, hold_creation = false, fix_differences = false;

    int mouseX = 0, mouseY = 0, game_return_code = -1, next_position_update = 0, last_colision_track = -1, last_colision_index = -1, point_acumulator = 10, losing_track = -1;;

    double speedX, speedY;

    double fps_start, fps_difference;

    struct MAP_INFO *map_info = NULL;

    ALLEGRO_BITMAP *background;
    ALLEGRO_BITMAP *cannon_bg = load_image("../img/game/cannon_bg.png");
    ALLEGRO_BITMAP *cannon = load_image("../img/game/cannon.png");

    initialize_generated_balls_at_color_array();

    load_level(&background, &map_info);

    /*
    for(int i = 0; i < map_info->map_length; i++)
    {
        for(int j = 0; j < map_info->tracks[i].track_length; j++)
        {
            POINT p = map_info->tracks[i].path[j];

            fprintf(stderr, "Ponto %d da track %d: %.2f %.2f\n", j, i, p.x, p.y);
        }
    }
    */

    PROJECTILE current_projectile,
               next_projectile,
               on_way_projectile;

    current_projectile.color = COLORS[rand() % (4 + ((LEVEL - 1) / 3))];
    next_projectile.color = COLORS[rand() % (4 + ((LEVEL - 1) / 3))];

    BALL *ball_at_track[map_info->map_length];

    int max_balls_per_track[map_info->map_length];
    int created_balls_at_track[map_info->map_length];

    for(int i = 0; i < map_info->map_length; i++)
    {
        max_balls_per_track[i] = (map_info->tracks[i].track_length / (GAME_PROJECTILE_RADIUS * 2));
        ball_at_track[i] = (BALL *) malloc(max_balls_per_track[i] * sizeof(BALL));
        created_balls_at_track[i] = 0;

        for(int j = 0; j < max_balls_per_track[i]; j++)
        {
            ball_at_track[i][j].position = -1;
        }
    }

    while(game_return_code == -1)
    {
        fps_start = al_get_time();
        // Início laço de atualização da tela

        while(!al_event_queue_is_empty(event_queue))
        {
            ALLEGRO_EVENT event;

            al_get_next_event(event_queue, &event);

            if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                bool answer = display_yes_no_confirmation(NULL, "Tem certeza?", "Você tem certeza que deseja sair?", "Todo o progresso será perdido");

                if(answer)
                {
                    game_return_code = GAME_RETURN_CODE_LEAVE;
                }
            }
            else if(event.type == ALLEGRO_EVENT_MOUSE_AXES)
            {
                mouseX = event.mouse.x;
                mouseY = event.mouse.y;
            }
            else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && !fired_projectile && !projectile_on_way && !hold_positions)
            {
                fired_projectile = true;

                double deltaX = mouseX - (GAME_WIDTH / 2);
                double deltaY = mouseY - (GAME_HEIGHT / 2);

                double rotation_angle = atan2(deltaX, deltaY) * -1 + M_PI;

                speedX = sin(rotation_angle) * PROJECTILE_SPEED;
                speedY = cos(rotation_angle) * -PROJECTILE_SPEED;
            }
        }

        double rotation_angle = atan2f(mouseX - (GAME_WIDTH / 2), mouseY - (GAME_HEIGHT / 2)) * -1 + M_PI;

        reset_screen(background, cannon_bg, cannon, rotation_angle, font);

        if(fired_projectile)
        {
            on_way_projectile = current_projectile;
            current_projectile = next_projectile;
            next_projectile.color = generate_random_color(true);
            projectile_on_way = true;
            fired_projectile = false;
        }

        current_projectile.cord.x = (GAME_WIDTH / 2) + (sin(rotation_angle) * 55);
        current_projectile.cord.y = (GAME_HEIGHT / 2) + (cos(rotation_angle) * -55);

        next_projectile.cord.x = (GAME_WIDTH / 2) + (sin(rotation_angle) * -35);
        next_projectile.cord.y = (GAME_HEIGHT / 2) + (cos(rotation_angle) * 35);

        al_draw_filled_circle(current_projectile.cord.x, current_projectile.cord.y, GAME_PROJECTILE_RADIUS, current_projectile.color);
        al_draw_filled_circle(next_projectile.cord.x, next_projectile.cord.y, GAME_NEXT_PROJECTILE_RADIUS, next_projectile.color);

        if(!hold_positions)
        {
            if(next_position_update <= 0)
            {
                next_position_update = (GAME_FPS / BALLS_SPEED);

                losing_track = advance_balls(map_info, ball_at_track, created_balls_at_track, 0);

                if(losing_track >= 0)
                {
                    game_return_code = GAME_RETURN_CODE_LOST;
                }
                else if(!hold_creation && BALLS_TO_COME > 0)
                {
                    for(int i = 0; i < map_info->map_length; i++)
                    {
                        if(created_balls_at_track[i] < max_balls_per_track[i] && (created_balls_at_track[i] == 0 || ball_at_track[i][0].position > (GAME_PROJECTILE_RADIUS * 2)))
                        {
                            create_ball_at_track(i, map_info, ball_at_track, created_balls_at_track);
                        }
                    }
                }
            }
            next_position_update--;
        }
        else
        {
            if(last_colision_track >= 0 && last_colision_index >= 0)
            {
                if(fix_differences)
                {
                    fix_differences = false;

                    for(int i = last_colision_index; i < created_balls_at_track[last_colision_track]; i++)
                    {
                        if(i < 1)
                        {
                            i++;
                        }

                        BALL *current = &ball_at_track[last_colision_track][i];
                        BALL *previous = &ball_at_track[last_colision_track][i - 1];

                        int distance = current->position - previous->position, deslocamento = 3;

                        if(distance < GAME_PROJECTILE_RADIUS * 2 && distance < -deslocamento)
                        {
                            current->position += deslocamento;
                            fix_differences = true;
                        }
                        else if(distance > GAME_PROJECTILE_RADIUS * 2 && distance > deslocamento)
                        {
                            current->position -= deslocamento;
                            fix_differences = true;
                        }
                        else
                        {
                            current->position = previous->position + (GAME_PROJECTILE_RADIUS * 2);
                        }
                    }
                }
                else
                {
                    hold_positions = false;

                    if(created_balls_at_track[last_colision_track] > 0)
                    {
                        bool destroyed_any;

                        int equal_balls_left = 0,
                            equal_balls_right = 0;

                        int i;

                        i = last_colision_index - 1;

                        while(i >= 0 && colors_match(ball_at_track[last_colision_track][i--].color, ball_at_track[last_colision_track][last_colision_index].color))
                        {
                            equal_balls_left++;
                        }

                        i = last_colision_index + 1;
                        while(i < created_balls_at_track[last_colision_track] && colors_match(ball_at_track[last_colision_track][i++].color, ball_at_track[last_colision_track][last_colision_index].color))
                        {
                            equal_balls_right++;
                        }

                        destroyed_any = equal_balls_left + equal_balls_right >= 2;

                        if(destroyed_any)
                        {
                            int position_last_ball = ball_at_track[last_colision_track][created_balls_at_track[last_colision_track] - 1].position;

                            ALLEGRO_COLOR destroyed_color = ball_at_track[last_colision_track][last_colision_index].color;

                            int total = equal_balls_left + equal_balls_right, removidas;

                            PLAYER_SCORE += total * point_acumulator;

                            total++;

                            last_colision_index = last_colision_index - equal_balls_left;

                            for(i = last_colision_index; i < created_balls_at_track[last_colision_track]; i++)
                            {
                                if(i + total < created_balls_at_track[last_colision_track])
                                {
                                    ball_at_track[last_colision_track][i] = ball_at_track[last_colision_track][i + total];
                                }
                                else
                                {
                                    break;
                                }
                            }
                            created_balls_at_track[last_colision_track] -= total;

                            if(last_colision_index == 0 && created_balls_at_track[last_colision_track] > 0)
                            {
                                ball_at_track[last_colision_track][0].position -= (total * GAME_PROJECTILE_RADIUS * 2);
                            }

                            if(BALLS_TO_COME == 0)
                            {
                                bool end_of_game = true;

                                for(int j = 0; j < map_info->map_length; j++)
                                {
                                    if(created_balls_at_track[j] > 0)
                                    {
                                        end_of_game = false;
                                        break;
                                    }
                                }

                                if(end_of_game)
                                {
                                    int c = 0;
                                    bool increment = false;

                                    while(position_last_ball < map_info->tracks[last_colision_track].track_length)
                                    {
                                        reset_screen(background, cannon_bg, cannon, rotation_angle, font);

                                        POINT prev = map_info->tracks[last_colision_track].path[position_last_ball - (GAME_PROJECTILE_RADIUS * 2)];
                                        POINT curr = map_info->tracks[last_colision_track].path[position_last_ball];

                                        al_draw_filled_circle(prev.x, prev.y, GAME_PROJECTILE_RADIUS, al_map_rgba(0,0,0,50));

                                        al_draw_filled_circle(curr.x, curr.y, GAME_PROJECTILE_RADIUS, al_map_rgb(0,0,0));

                                        al_flip_display();

                                        position_last_ball += (GAME_PROJECTILE_RADIUS);

                                        if(increment)
                                        {
                                            PLAYER_SCORE += 7;
                                        }
                                        
                                        al_rest(0.03);

                                        increment = !increment;
                                    }

                                    reset_screen(background, cannon_bg, cannon, rotation_angle, font);
                                    al_flip_display();

                                    al_rest(2);

                                    game_return_code = GAME_RETURN_CODE_WON;
                                }
                            }
                            
                            if(game_return_code != GAME_RETURN_CODE_WON)
                            {
                                for(int j = 0; j < AMOUNT_OF_COLORS; j++)
                                {
                                    if(colors_match(COLORS[j], destroyed_color))
                                    {
                                        GENERATED_BALLS_OF_COLOR[j] -= total;

                                        if(GENERATED_BALLS_OF_COLOR[j] <= 0)
                                        {
                                            if(colors_match(current_projectile.color, COLORS[j]))
                                            {
                                                current_projectile.color = generate_random_color(true);
                                            }

                                            if(colors_match(next_projectile.color, COLORS[j]))
                                            {
                                                next_projectile.color = generate_random_color(true);
                                            }
                                        }
                                    }
                                }
                            }
                            

                            if(created_balls_at_track[last_colision_track] < 0)
                            {
                                created_balls_at_track[last_colision_track] = 0;
                                hold_positions = false;
                                fix_differences = false;
                            }
                            else
                            {
                                fix_differences = true;
                                hold_positions = true;
                            }
                        }
                    }
                }
            }

            if(!hold_positions)
            {
                point_acumulator = 10;
                last_colision_track = -1;
                last_colision_index = -1;
            }
        }
        
        draw_balls(map_info, ball_at_track, created_balls_at_track);

        if(projectile_on_way)
        {
            if(on_way_projectile.cord.x > GAME_WIDTH || on_way_projectile.cord.y > GAME_HEIGHT || on_way_projectile.cord.x < 0 || on_way_projectile.cord.y < 0)
            {
                PLAYER_SCORE -= 7;
                projectile_on_way = false;
            }
            else
            {
                double distance, best_distance = GAME_NEXT_PROJECTILE_RADIUS * 2 + 1;

                int colision_track = -1;
                int colision_index = -1;
                bool hit_at_left;

                BALL *colision_ball = NULL;

                for(int i = 0; i < map_info->map_length; i++)
                {
                    for(int j = 0; j < created_balls_at_track[i]; j++)
                    {
                        colision_ball = &ball_at_track[i][j];

                        POINT p = map_info->tracks[i].path[colision_ball->position];

                        if(p.ball_vulnerable == 1 && (distance = distance_between_points(on_way_projectile.cord.x, on_way_projectile.cord.y, p.x, p.y)) <= GAME_NEXT_PROJECTILE_RADIUS * 2 + 20 && distance < best_distance)
                        {
                            best_distance = distance;

                            hit_at_left = on_way_projectile.cord.x - p.x < 0;

                            colision_track = i;
                            colision_index = j;
                        }
                    }
                }

                if(colision_track >= 0)
                {  
                    colision_ball = &ball_at_track[colision_track][colision_index];

                    projectile_on_way = false;
                    hold_positions = true;

                    BALL new_ball;

                    new_ball.position = colision_ball->position;
                    new_ball.color = on_way_projectile.color;

                    for(int i = created_balls_at_track[colision_track]; i > colision_index + 1 && i > 0; i--)
                    {
                        ball_at_track[colision_track][i] = ball_at_track[colision_track][i - 1];
                    }

                    if(hit_at_left)
                    {
                        ball_at_track[colision_track][colision_index + 1] = ball_at_track[colision_track][colision_index];
                        ball_at_track[colision_track][colision_index] = new_ball;
                    }
                    else
                    {
                        ball_at_track[colision_track][colision_index + 1] = new_ball;
                        colision_index++;
                    }

                    last_colision_track = colision_track;
                    last_colision_index = colision_index;
                    
                    created_balls_at_track[colision_track]++;
                
                    for(int i = 0; i < AMOUNT_OF_COLORS; i++)
                    {
                        if(colors_match(COLORS[i], new_ball.color))
                        {
                            GENERATED_BALLS_OF_COLOR[i]++;
                        }
                    }

                    for(int i = 0; i < map_info->map_length; i++)
                    {
                        int acumulador = ball_at_track[i][0].position;

                        for(int j = 1; j < created_balls_at_track[i]; j++)
                        {
                            acumulador += (GAME_PROJECTILE_RADIUS * 2);

                            ball_at_track[i][j].position = acumulador;
                        }
                    }
                }
                else
                {
                    on_way_projectile.cord.x += speedX;
                    on_way_projectile.cord.y += speedY;

                    al_draw_filled_circle(on_way_projectile.cord.x, on_way_projectile.cord.y, GAME_PROJECTILE_RADIUS, on_way_projectile.color);
                }
                
            }
        } 

        if(game_return_code == GAME_RETURN_CODE_LOST)
        {
            int soma_bolinhas = 0;

            do 
            {
                soma_bolinhas = 0;

                reset_screen(background, cannon_bg, cannon, rotation_angle, font);

                advance_balls(map_info, ball_at_track, created_balls_at_track, 3);

                draw_balls(map_info, ball_at_track, created_balls_at_track);
                
                al_flip_display();

                for(int i = 0; i < map_info->map_length; i++)
                {
                    soma_bolinhas += created_balls_at_track[i];
                }

            } while(soma_bolinhas > 0);

            ALLEGRO_BITMAP *explosion = load_image("../img/game/explosion.png");

            if(explosion != NULL)
            {
                for(int i = 0; i < map_info->map_length; i++)
                {
                    POINT p = map_info->tracks[i].path[map_info->tracks[i].track_length - 1];

                    al_draw_bitmap(explosion, p.x - 150, p.y - 150, 0);
                }
                
                al_destroy_bitmap(explosion);
            }

            al_flip_display();

            al_rest(1);
        }
        else
        {
            al_flip_display();
        }

        // Final laço de atualização da tela
        fps_difference = al_get_time() - fps_start;

        if(fps_difference < 1.0 / GAME_FPS)
        {
            al_rest((1.0 / GAME_FPS) - fps_difference);
        }
    }

    al_destroy_bitmap(background);
    al_destroy_bitmap(cannon_bg);
    al_destroy_bitmap(cannon);

    for(int i = 0; i < map_info->map_length; i++)
    {
        free(ball_at_track[i]);
        free(map_info->tracks[i].path);
    }

    free(map_info->tracks);
    free(map_info);

    write_log(DEBUG_LEVEL_ALL, true, "Deixando a função play.");

    return game_return_code;
}