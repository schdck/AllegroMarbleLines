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
        al_map_rgb(255, 0, 0),
        al_map_rgb(0, 255, 0),
        al_map_rgb(0, 0, 255)
    };

    return colors[rand() % AMOUNT_OF_COLORS];
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
        fprintf(stderr, "%d, %d\n", point_x, point_y);

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

    bool fired_ball = false, ball_on_way = false, hold = false;

    int mouseX = 0, mouseY = 0, game_return_code = -1, ultima_atualizacao_posicao = 0;

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
               on_way_projectile,
               *projectiles_at_track[map_info->map_length];

    current_projectile.color = GenerateRandomColor();
    next_projectile.color = GenerateRandomColor();

    int projectiles_per_track[map_info->map_length];
    int created_projectiles[map_info->map_length];

    for(int i = 0; i < map_info->map_length; i++)
    {
        projectiles_per_track[i] = (map_info->tracks[i].track_length / (GAME_PROJECTILE_RADIUS * 2));
        projectiles_at_track[i] = (PROJECTILE *) malloc(projectiles_per_track[i] * sizeof(PROJECTILE));
        created_projectiles[i] = 0;

        for(int j = 0; j < projectiles_per_track[i]; j++)
        {
            projectiles_at_track[i][j].cord.x = -1;
            projectiles_at_track[i][j].cord.y = -1;

            write_log(DEBUG_LEVEL_ALL, true, "Inicializado ponto %d da trilha %d para as coordenadas: %.2f, %.2f", j, i, projectiles_at_track[i][j].cord.x, projectiles_at_track[i][j].cord.y);
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
            next_projectile.color = GenerateRandomColor();
            ball_on_way = true;
            fired_ball = false;
        }

        current_projectile.cord.x = (GAME_WIDTH / 2) + (sin(rotation_angle) * 55);
        current_projectile.cord.y = (GAME_HEIGHT / 2) + (cos(rotation_angle) * -55);

        next_projectile.cord.x = (GAME_WIDTH / 2) + (sin(rotation_angle) * -35);
        next_projectile.cord.y = (GAME_HEIGHT / 2) + (cos(rotation_angle) * 35);

        al_draw_filled_circle(current_projectile.cord.x, current_projectile.cord.y, GAME_PROJECTILE_RADIUS, current_projectile.color);
        al_draw_filled_circle(next_projectile.cord.x, next_projectile.cord.y, GAME_NEXT_PROJECTILE_RADIUS, next_projectile.color);

        if(ball_on_way)
        {
            if(on_way_projectile.cord.x > GAME_WIDTH || on_way_projectile.cord.y > GAME_HEIGHT || on_way_projectile.cord.x < 0 || on_way_projectile.cord.y < 0)
            {
                PLAYER_SCORE -= 7;
                ball_on_way = false;
            }
            else
            {
                bool colision_detected = false;

                int colision_track = -1, colision_index = -1;

                for(int i = 0; i < map_info->map_length; i++)
                {
                    for(int j = 0; j < map_info->tracks[j].track_length; j++)
                    {
                        for(int k = 0; k < created_projectiles[j]; k++)
                        {
                            POINT p = projectiles_at_track[j][k].cord;

                            if(distance_between_points(on_way_projectile.cord.x, on_way_projectile.cord.y, p.x, p.y) < GAME_NEXT_PROJECTILE_RADIUS * 2)
                            {
                                colision_detected = true;
                                colision_track = j;
                                colision_index = k;
                            }
                        }
                    }
                }

                /*if(colision_detected)
                {
                    ball_on_way = false;

                    PROJECTILE *last_projectile = &projectiles_at_track[colision_track][created_projectiles[colision_track] - 1];
                    
                    POINT start_point = last_projectile->cord;
                    POINT current_point = last_projectile->cord;
                    double distance;

                    do
                    {
                        *last_projectile->position++;

                        map_info->tracks[colision_track].path[last_projectile->position]

                        current_point.x = map_info->tracks[colision_track].path[last_projectile->position].x;
                        current_point.y = map_info->tracks[colision_track].path[last_projectile->position].y;
                        
                    }
                    while(distance_between_points(start_point.x, start_point.y, current_point.x, current_point.y) < GAME_PROJECTILE_RADIUS * 2 && created_projectiles[i] < projectiles_per_track[i])

                    for(int i = created_projectiles[colision_track]; i > colision_index; i--)
                    {
                        projectiles_at_track[colision_track][i] = projectiles_at_track[colision_track][i - 1];
                    }

                    projectiles_at_track[colision_track][colision_index] = ball
                }
                else
                {
                    */
                    on_way_projectile.cord.x += speedX;
                    on_way_projectile.cord.y += speedY;

                    al_draw_filled_circle(on_way_projectile.cord.x, on_way_projectile.cord.y, GAME_PROJECTILE_RADIUS, on_way_projectile.color);
                //}
                
            }
        }

        if(!hold)
        {
            if(ultima_atualizacao_posicao <= 0)
            {
                ultima_atualizacao_posicao = GAME_FPS / BALLS_SPEED;

                for(int i = 0; i < map_info->map_length; i++)
                {
                    PROJECTILE *p;

                    for(int j = 0; j < created_projectiles[i]; j++)
                    {
                        p = &projectiles_at_track[i][j];

                        p->position++;

                        if(p->position == map_info->tracks[i].track_length)
                        {
                            fprintf(stderr, "GAME OVER!\n");
                        }
                        else
                        {
                            p->cord = map_info->tracks[i].path[p->position];
                        }
                    }

                    bool create_new_projectile = false;

                    if(created_projectiles[i] > 0)
                    {
                        POINT n = projectiles_at_track[i][0].cord,
                              s = map_info->tracks[i].path[0];

                        double distance = distance_between_points(s.x, s.y, n.x, n.y);

                        //fprintf(stderr, "d((%.2f, %.2f), (%.2f, %.2f)) = %.2f\n", s.x, s.y, n.x, n.y, distance_between_points(s.x, s.y, n.x, n.y));

                        if(distance > GAME_PROJECTILE_RADIUS * 2 && created_projectiles[i] < projectiles_per_track[i])
                        {
                            for(int j = created_projectiles[i]; j > 0; j--)
                            {
                                write_log(DEBUG_LEVEL_ALL, true, "Deslocando projétil da trilha %d da posição %d para a posição %d", i, j, j - 1);

                                projectiles_at_track[i][j] = projectiles_at_track[i][j - 1];
                            }

                            create_new_projectile = true;
                        }                   
                    }
                    
                    if(created_projectiles[i] == 0 || create_new_projectile)
                    {
                        p = &projectiles_at_track[i][0];
                        p->position = 0;
                        p->color = GenerateRandomColor();
                        p->cord.x = map_info->tracks[i].path[0].x;
                        p->cord.y = map_info->tracks[i].path[0].y;
                        created_projectiles[i]++;

                        write_log(DEBUG_LEVEL_ALL, true, "Novo projétil da track %d carregado para %.2f, %.2f", i, p->cord.x, p->cord.y);
                    }
                }
            }

            ultima_atualizacao_posicao--;
        }
            

        for(int i = 0; i < map_info->map_length; i++)
        {
            for(int j = 0; j < projectiles_per_track[i]; j++)
            {
                PROJECTILE *p = &projectiles_at_track[i][j];
                
                if(p->cord.x < 0 && p->cord.y < 0)
                {
                    break;
                }

                al_draw_filled_circle(p->cord.x, p->cord.y, GAME_PROJECTILE_RADIUS, p->color);
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
        free(projectiles_at_track[i]);
        free(map_info->tracks[i].path);
    }

    free(map_info->tracks);
    free(map_info);

    write_log(DEBUG_LEVEL_ALL, true, "Deixando a função Jogar.");

    return game_return_code;
}