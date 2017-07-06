#ifndef __ALLEGRO_UTILS_H_
#define __ALLEGRO_UTILS_H_

void DisplayError(ALLEGRO_DISPLAY *display, const char *title, const char *subtitle, const char *message);

bool DisplayYesNoConfirmation(ALLEGRO_DISPLAY *display, const char *title, const char *subtitle, const char *message);

bool colors_match(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2);

int InicializarAllegro();

struct ALLEGRO_BITMAP *load_image(const char *path);

#endif // __ALLEGRO_UTILS_H_