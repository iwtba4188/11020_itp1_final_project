#include "global.h"
#include "charater.h"

void menu_init();
void menu_process(ALLEGRO_EVENT event);
void menu_draw();
void menu_destroy();

void game_scene1_init();
void game_scene1_draw();
void game_scene1_destroy();

void setting_scene_init();
void setting_scene_process(ALLEGRO_EVENT event);
void setting_scene_draw();
void setting_scene_destory();

void about_scene_init();
void about_scene_process(ALLEGRO_EVENT event);
void about_scene_draw();
void about_scene_destroy();

void exit_fighting_scene_init();
void exit_fighting_scene_process(ALLEGRO_EVENT event);
void exit_fighting_scene_draw();
void exit_fighting_scene_destory();

void create_scene_map_img(char* map_file_name, char* save_img_name);