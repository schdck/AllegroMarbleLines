#ifndef __GAME_FUNCTIONS_H_
#define __GAME_FUNCTIONS_H_

struct POINT {
    double x;
    double y;

    int ball_vulnerable;
};

struct PROJECTILE {
    ALLEGRO_COLOR color;
    POINT cord;
};

struct BALL {
    ALLEGRO_COLOR color;
    int position;
};

struct TRACK {
    POINT *path;
    int track_length;
};

struct MAP_INFO {
    TRACK *tracks;
    int map_length;
};

void exibir_janela_jogo(int X, int Y, ushort game_mode, ushort level, ushort start_amout_of_balls, ushort balls_speed);

#endif // __GAME_FUNCTIONS_H_