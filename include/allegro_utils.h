#ifndef __ALLEGRO_UTILS_H_
#define __ALLEGRO_UTILS_H_

void display_error(ALLEGRO_DISPLAY *display, const char *title, const char *subtitle, const char *message);

bool display_yes_no_confirmation(ALLEGRO_DISPLAY *display, const char *title, const char *subtitle, const char *message);

bool colors_match(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2);

int inicializar_allegro5();

struct ALLEGRO_BITMAP *load_image(const char *path);

#endif // __ALLEGRO_UTILS_H_