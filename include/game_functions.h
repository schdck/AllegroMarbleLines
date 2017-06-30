#ifndef __GAME_FUNCTIONS_H_
#define __GAME_FUNCTIONS_H_

struct PROJECTILE {
    double x;
    double y;
    ALLEGRO_COLOR color;
};

void ExibirJanelaJogo(int X, int Y, ushort game_mode, ushort level, ushort projectile_speed, ushort balls_speed);

#endif // __GAME_FUNCTIONS_H_