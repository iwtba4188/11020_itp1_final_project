#include "global.h"
#include "charater.h"

void menu_init();
void menu_process(ALLEGRO_EVENT event);
void menu_draw();
void menu_destroy();

void game_scene1_init();
void game_scene1_draw();
void game_scene1_destroy();

void create_scene_map_img(char* map_file_name, char* save_img_name);