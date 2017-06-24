#ifndef __ALLEGRO_UTILS_H_
#define __ALLEGRO_UTILS_H_

void DisplayError(ALLEGRO_DISPLAY *display, const char *title, const char *subtitle, const char *message);

int InicializarAllegro();

struct ALLEGRO_BITMAP *load_image(const char *path);

#endif // __ALLEGRO_UTILS_H_