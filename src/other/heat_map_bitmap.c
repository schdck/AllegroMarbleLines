#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#define		FRONT		0
#define		BACK		1
#define		LEFT		2
#define		RIGHT		3
#define     NORDESTE    4
#define     SUDESTE     5
#define     SUDOESTE    6
#define     NOROESTE    7

#define IMAGE_WIDTH     800
#define IMAGE_HEIGHT    600

#define PLOT_POINTS     0

struct POINT {
    int x;
    int y;
    int vulnerable;

    POINT *next;
    POINT *prev;
};

struct LIST {
    POINT *first;
    POINT *last;
};

ALLEGRO_BITMAP *IMAGE;

LIST *POINTS = (LIST *) malloc(sizeof(LIST));

bool PASSED[IMAGE_WIDTH][IMAGE_HEIGHT];

int PIXELS_FOUND = 0;

POINT *criar_ponto(int x, int y, int vulnerable)
{
    struct POINT *p = (POINT *) malloc(sizeof(POINT));

    p->vulnerable = vulnerable;
	p->x = x;
    p->y = y;
	p->prev = NULL;
	p->next = NULL;
    
    return p;
}

void inserir_no_fim(POINT *p)
{
    if(POINTS->first == NULL)
    {
        POINTS->first = p;
    }

    if(POINTS->last == NULL) 
    {
		POINTS->last = p;
		return;
    }

    p->prev = POINTS->last;
    POINTS->last->next = p;
    POINTS->last = p;
}

int get_oposite_direction(int d)
{
    switch(d)
    {
        case FRONT: return BACK;
        case BACK: return FRONT;
        case LEFT: return RIGHT;
        case RIGHT: return LEFT;
        case SUDESTE: return NOROESTE;
        case SUDOESTE: return NORDESTE;
        case NORDESTE: return SUDOESTE;
        case NOROESTE: return SUDESTE;
    }

    return -1;
}

void map_image(int startX, int startY, int direcao)
{
	if(direcao == FRONT)
	{
		startY++;
	}
	else if(direcao == BACK)
	{
		startY--;
	}
	else if(direcao == LEFT)
	{
		startX--;
	}
	else if(direcao == RIGHT)
	{
		startX++;
	}
    else if(direcao == NORDESTE)
    {
        startY--;
        startX++;
    }
    else if(direcao == NOROESTE)
    {
        startY--;
        startX--;
    }
    else if(direcao == SUDESTE)
    {
        startY++;
        startX++;
    }
    else if(direcao == SUDOESTE)
    {
        startY++;
        startX--;
    }
	else
	{
		printf("Direção inválida!\n");
		return;
	}

    ALLEGRO_COLOR c = al_get_pixel(IMAGE, startX, startY);

	if(startX < 0 || startX >= IMAGE_WIDTH || startY < 0 || startY >= IMAGE_HEIGHT || (c.r < 0.5 && c.g < 0.5 && c.b < 0.5) || PASSED[startX][startY])
	{
		return;
	}
	
    PIXELS_FOUND++;
    PASSED[startX][startY] = true;

    int vulnerable;

    if(c.r > c.g && c.r > c.b)
    {
        vulnerable = 1;
    }
    else if(c.g > c.r && c.g > c.b)
    {
        vulnerable = 0;
    }
    else
    {
        vulnerable = 2;
    }

    inserir_no_fim(criar_ponto(startX, startY, vulnerable));

    al_draw_pixel(startX, startY, c);
    
    if(PLOT_POINTS)
    {
        al_flip_display();
    }

    for(int i = FRONT; i <= NOROESTE; i++)
    {
        if(i != get_oposite_direction(direcao))
        {
            map_image(startX, startY, i);
        }
    }
}

void map_image(int startX, int startY)
{
    for(int i = FRONT; i < SUDOESTE; i++)
    {
        map_image(startX, startY, i);
    }
}

int main()
{
    if(!al_init()) 
    {
        fprintf(stderr, "Erro inicializando o Allegro.\n");
        exit(EXIT_FAILURE);
    }

    if(!al_init_image_addon())
    {
        fprintf(stderr, "Erro inicializando a extensão de imagens.\n");
        exit(EXIT_FAILURE);
    }

    IMAGE = al_load_bitmap("/home/mvschmaedeck/Documents/AllegroMarbleLines/design/heatmaps/level_3_2.png");

    if(IMAGE == NULL)
    {
        fprintf(stderr, "Imagem não encontrada.\n");
        exit(EXIT_FAILURE);
    }

    if(IMAGE_WIDTH != al_get_bitmap_width(IMAGE) || IMAGE_HEIGHT != al_get_bitmap_height(IMAGE))
    {
        fprintf(stderr, "Imagem não bate com as dimensões.");
        exit(EXIT_FAILURE);
    }

    ALLEGRO_DISPLAY *display = al_create_display(IMAGE_WIDTH, IMAGE_HEIGHT);    

    bool found_start;

    int x, y;

    al_draw_bitmap(IMAGE, 0, 0, 0);

    for(y = IMAGE_HEIGHT; y >= 0 && !found_start; y--)
    {
        for(x = 0; x < IMAGE_WIDTH && !found_start; x++)
        {
            PASSED[x][y] = true;

            al_draw_pixel(x, y, al_map_rgb(255,255,255));

            if(al_get_pixel(IMAGE, x, y).r > 0.5 || al_get_pixel(IMAGE, x, y).b > 0.5)
            {
                found_start = true;
            }
        }

        al_flip_display();
    }

    al_rest(3);

    al_clear_to_color(al_map_rgb(0,0,0));

    map_image(x, y);

    fprintf(stderr, "Pixels found: %d\n", PIXELS_FOUND);

    al_destroy_bitmap(IMAGE);

    al_flip_display();

    FILE *file;

    if((file = fopen("output.csv", "wb")) == NULL)
    {
        fprintf(stderr, "Erro ao abrir/criar arquivo.\n");
        exit(EXIT_FAILURE);
    }

    struct POINT* current = POINTS->first;

	while(current != NULL)
    {
		//fprintf(stderr, "Escrevendo ponto %d %d (vulnerable: %s) para o arquivo.\n", current->x, current->y, current->vulnerable ? "true" : "false");
        fprintf(file, "%d,%d,%d\n",current->x, current->y, current->vulnerable);
		current = current->next;
	}
    fprintf(file, "-1,-1,-1");

    fclose(file);

    al_rest(3);

    al_destroy_display(display);
}