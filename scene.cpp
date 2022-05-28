#include <stdlib.h>
#include <string.h>

#include "scene.h"

const int BLOCK_IN_ROW = HEIGHT / BLOCK_HEIGHT;
const int BLOCK_IN_COL = WIDTH / BLOCK_WIDTH;
ALLEGRO_FONT* font = NULL;
ALLEGRO_BITMAP* scene1_img = NULL;

// function of menu
void menu_init() {
    font = al_load_ttf_font("./font/pirulen.ttf", 12, 0);
}
void menu_process(ALLEGRO_EVENT event) {
    if (event.type == ALLEGRO_EVENT_KEY_UP)
        if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) judge_next_window = true;
}
void menu_draw() {
    al_clear_to_color(al_map_rgb(100, 100, 100));
    al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 220, ALLEGRO_ALIGN_CENTRE, "Press 'Enter' to start");
    al_draw_rectangle(WIDTH / 2 - 150, 510, WIDTH / 2 + 150, 550, al_map_rgb(255, 255, 255), 0);
}
void menu_destroy() {
    al_destroy_font(font);
}

// function of game_scene
void game_scene1_init() {
    // XXX: object init
    character_init();
    monster_init();
    camera_update();
    // background = al_load_bitmap("./image/stage.jpg");
    create_scene_map_img("./map/scene1.txt", "./map/scene1.jpg");
    scene1_img = al_load_bitmap("./map/scene1.jpg");
}
void game_scene1_draw() {
    // printf("x%d y%d\n", OBJECT_MODIFY_X, OBJECT_MODIFY_Y);
    // scanf("%d", OBJECT_MODIFY_X);
    al_clear_to_color(al_map_rgb(255, 255, 255));
    al_draw_bitmap(scene1_img, OBJECT_MODIFY_X, OBJECT_MODIFY_Y, 0);
    // XXX: object draw
    character_draw();
    bullets_draw();
    monster_draw();
    objects_draw();
}
void game_scene1_destroy() {
    al_destroy_bitmap(scene1_img);
    // XXX: object destory
    character_destory();
    bullets_destory();
    monster_destory();
}

// my scene utilities
void create_scene_map_img(char* map_file_name, char* save_img_name) {
    FILE* file;
    file = fopen(map_file_name, "r");

    fscanf(file, "%d %d", &THIS_SCENE_SIZE_ROW, &THIS_SCENE_SIZE_COL);

    ALLEGRO_BITMAP* new_map;
    new_map = al_create_bitmap(THIS_SCENE_SIZE_COL * BLOCK_WIDTH, THIS_SCENE_SIZE_ROW * BLOCK_HEIGHT);
    al_set_target_bitmap(new_map);
    for (int i = 0; i < THIS_SCENE_SIZE_ROW; i++) {
        for (int j = 0; j < THIS_SCENE_SIZE_COL; j++) {
            int block_num = 0;
            char scene_block_path[50];
            fscanf(file, "%d", &block_num);
            THIS_SCENE_MAP[i][j] = block_num;

            // draw map
            if (block_num == 1 || block_num == 3) {
                sprintf(scene_block_path, "./image/%d.png", 1);
            } else if (block_num == 2) {    // wall
                sprintf(scene_block_path, "./image/%d.png", 2);
            } else {
                sprintf(scene_block_path, "./image/%d.png", 0);
            }
            al_draw_bitmap(al_load_bitmap(scene_block_path), BLOCK_WIDTH * j, BLOCK_HEIGHT * i, 0);
        }
    }
    fclose(file);
    al_save_bitmap(save_img_name, new_map);
    al_destroy_bitmap(new_map);
    printf("[fin] create_scene_map_img...\n");
}
