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
#include "../include/game_functions.h"

static ushort PROJECTILE_SPEED;
static ushort BALLS_SPEED;
static ushort GAME_MODE;
static ushort LEVEL;
static int PLAYER_SCORE;

int Jogar(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_FONT *font);

struct ALLEGRO_COLOR GenerateRandomColor()
{
    const int AMOUNT_OF_COLORS = 3;

    ALLEGRO_COLOR colors[AMOUNT_OF_COLORS] = {
        al_map_rgba(255, 0, 0, 20),
        al_map_rgba(0, 255, 0, 20),
        al_map_rgba(0, 0, 255, 20)
    };

    return colors[rand() % AMOUNT_OF_COLORS];
}

struct BALL generate_ball()
{
    BALL b;

    b.position = 0;
    b.color = GenerateRandomColor();

    return b;
}

void shift_array(BALL *a, int n, int d) 
{
   for (int i = n; i >= 0 + d; i--)
   {
       *(a + i) = *(a + i - d);
       *(a + i) = *(a + i - d);
   }
}

void ExibirJanelaJogo(int X, int Y, ushort game_mode, ushort level, ushort projectile_speed, ushort balls_speed)
{
    write_log(DEBUG_LEVEL_ALL, true, "Iniciando a função ExibirJanelaJogo.");

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

    int game_return_code;

    while((game_return_code = Jogar(game_display, game_event_queue, game_font)) == GAME_RETURN_CODE_WON)
    {
        // XXX Exibir informações (posição no ranking, pontos, etc.)
        LEVEL++;
    }

    if(game_return_code == GAME_RETURN_CODE_LOST)
    {
        // XXX Oferecer opção líderes
    }

    al_destroy_font(game_font);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);

    write_log(DEBUG_LEVEL_ALL, true, "Deixando a função ExibirJanelaJogo.");
}

void CarregarLevel(struct ALLEGRO_BITMAP **level_background, struct MAP_INFO **map_info)
{
    write_log(DEBUG_LEVEL_ALL, true, "Iniciando a função CarregarLevel.");

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
        case 3:
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

    int position = 0, track = 0, point_x, point_y;

    POINT *p = &(*map_info)->tracks[0].path[0];

    while(fscanf(level_info, "%d,%d", &point_x, &point_y) == 2)
    {
        if(point_x >= 0 && point_y >= 0)
        {
            p = &(*map_info)->tracks[track].path[position++];

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

    write_log(DEBUG_LEVEL_ALL, true, "Deixando a função CarregarLevel.");
}

int Jogar(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_FONT *font)
{
    write_log(DEBUG_LEVEL_ALL, true, "Iniciando a função Jogar.");

    bool fired_projectile = false, projectile_on_way = false, hold_positions = false, hold_creation = false, fix_differences = false;

    int mouseX = 0, mouseY = 0, game_return_code = -1, last_position_update = 0, last_colision_track = -1, last_colision_index = -1, point_acumulator = 10;

    double speedX, speedY;

    double fps_start, fps_difference;

    struct MAP_INFO *map_info = NULL;

    ALLEGRO_BITMAP *background;
    ALLEGRO_BITMAP *cannon_bg = load_image("../img/game/cannon_bg.png");
    ALLEGRO_BITMAP *cannon = load_image("../img/game/cannon.png");

    CarregarLevel(&background, &map_info);

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

    current_projectile.color = GenerateRandomColor();
    next_projectile.color = GenerateRandomColor();

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

            al_wait_for_event(event_queue, &event);

            if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                bool answer = DisplayYesNoConfirmation(NULL, "Tem certeza?", "Você tem certeza que deseja sair?", "Todo o progresso será perdido");

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
            else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && !fired_projectile && !projectile_on_way)
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

        al_clear_to_color(al_map_rgb(101,101,101));
        al_draw_bitmap(background, 0, 0, 0);

        al_draw_line(GAME_WIDTH / 2, GAME_HEIGHT / 2, mouseX, mouseY, al_map_rgb(255,0,0), 3.0);
        al_draw_bitmap(cannon_bg, GAME_WIDTH / 2 - 53, GAME_HEIGHT / 2 - 53, 0);
        al_draw_rotated_bitmap(cannon, 53, 53, GAME_WIDTH / 2, GAME_HEIGHT / 2, rotation_angle - M_PI, 0);
        
        al_draw_multiline_textf(font, al_map_rgb(0, 0, 0), mouseX + 15, mouseY + 15, 300.0, 12.0, ALLEGRO_ALIGN_LEFT, "angle %.2frad (%.2fdeg)\nsin %.2f\ncos %.2f", rotation_angle, rotation_angle * 57.2958, sin(rotation_angle), cos(rotation_angle));
        al_draw_multiline_textf(font, al_map_rgb(0, 0, 0), 5, 5, 300.0, 12.0, ALLEGRO_ALIGN_LEFT, "NÍVEL: %d\nPONTOS: %d", LEVEL, PLAYER_SCORE);

        if(fired_projectile)
        {
            on_way_projectile = current_projectile;
            current_projectile = next_projectile;
            next_projectile.color = GenerateRandomColor();
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
            if(last_position_update <= 0)
            {
                last_position_update = GAME_FPS / BALLS_SPEED;

                for(int i = 0; i < map_info->map_length; i++)
                {
                    for(int j = 0; j < created_balls_at_track[i]; j++)
                    {
                        BALL *b = &ball_at_track[i][j];

                        b->position++;

                        if(b->position == map_info->tracks[i].track_length)
                        {
                            fprintf(stderr, "GAME OVER!\n");
                        }
                    }

                    bool create_new_ball = created_balls_at_track[i] == 0 ||
                                           (created_balls_at_track[i] < max_balls_per_track[i] && ball_at_track[i][0].position > (GAME_PROJECTILE_RADIUS * 2));
                    
                    if(!hold_creation && create_new_ball)
                    {
                        if(created_balls_at_track[i] > 0)
                        {
                            shift_array(ball_at_track[i], created_balls_at_track[i], 1);
                        }

                        ball_at_track[i][0] = generate_ball();

                        created_balls_at_track[i]++;
                    }
                }
            }

            last_position_update--;
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

                        while(i > 0 && colors_match(ball_at_track[last_colision_track][i--].color, ball_at_track[last_colision_track][last_colision_index].color))
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
            
        for(int i = 0; i < map_info->map_length; i++)
        {
            for(int j = 0; j < created_balls_at_track[i]; j++)
            {
                BALL b = ball_at_track[i][j];

                POINT p = map_info->tracks[i].path[b.position];
                
                al_draw_filled_circle(p.x, p.y, GAME_PROJECTILE_RADIUS, b.color);
                al_draw_text(font, al_map_rgb(0, 0, 0), p.x, p.y, 0, convert_int(j));
            }
        }

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

                        if((distance = distance_between_points(on_way_projectile.cord.x, on_way_projectile.cord.y, p.x, p.y)) <= GAME_NEXT_PROJECTILE_RADIUS * 2 && distance < best_distance)
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

        al_flip_display();

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

    write_log(DEBUG_LEVEL_ALL, true, "Deixando a função Jogar.");

    return game_return_code;
}