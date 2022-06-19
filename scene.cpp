#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "scene.h"

enum { MENU, FIGHT, SETTING, ABOUT, EXIT, EXIT_PAGE, TITLE_RED };

const int BLOCK_IN_ROW = HEIGHT / BLOCK_HEIGHT;
const int BLOCK_IN_COL = WIDTH / BLOCK_WIDTH;
ALLEGRO_BITMAP* scene1_img = NULL;
int now_on = -1;

ALLEGRO_MOUSE_STATE mouse_state_;
ALLEGRO_BITMAP* background;
ALLEGRO_BITMAP* title_[2];
bool title_red = false;
ALLEGRO_BITMAP* start_game[2];
ALLEGRO_BITMAP* setting[2];
ALLEGRO_BITMAP* about[2];
ALLEGRO_BITMAP* exit_scene[2];
// function of menu
void menu_init() {
    font = al_load_ttf_font("./font/pirulen.ttf", 12, 0);
    background = al_load_bitmap("./image/background.jpg");
    title_[0] = al_load_bitmap("./image/title.png");
    title_[1] = al_load_bitmap("./image/title_red_ma.png");
    start_game[0] = al_load_bitmap("./image/start_game_0.png");
    start_game[1] = al_load_bitmap("./image/start_game_1.png");
    setting[0] = al_load_bitmap("./image/setting_0.png");
    setting[1] = al_load_bitmap("./image/setting_1.png");
    about[0] = al_load_bitmap("./image/about_0.png");
    about[1] = al_load_bitmap("./image/about_1.png");
    exit_scene[0] = al_load_bitmap("./image/exit_0.png");
    exit_scene[1] = al_load_bitmap("./image/exit_1.png");
}
void menu_process(ALLEGRO_EVENT event) {
    al_get_mouse_state(&mouse_state_);
    if (mouse_state_.buttons & 1 && now_on == FIGHT) {
        aaa_scene = FIGHT;
        last_scene = MENU;
        judge_next_window = true;
    } else if (mouse_state_.buttons & 1 && now_on == SETTING) {
        aaa_scene = SETTING;
        last_scene = MENU;
        judge_next_window = true;
    } else if (mouse_state_.buttons & 1 && now_on == ABOUT) {
        aaa_scene = ABOUT;
        last_scene = MENU;
        judge_next_window = true;
    } else if (mouse_state_.buttons & 1 && now_on == EXIT) {
        terminate = true;
    } else if (mouse_state_.buttons & 1 && now_on == TITLE_RED) {
        title_red = true;
    }
    // if (event.type == ALLEGRO_EVENT_KEY_UP)
    //     if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) judge_next_window = true;
}
void menu_draw() {
    al_draw_bitmap(background, 0, 0, 0);

    const int IMAGE_HEIGHT = al_get_bitmap_height(start_game[0]);

    int title_x = WIDTH / 4 - al_get_bitmap_width(title_[0]) / 2, title_y = HEIGHT / 2 - al_get_bitmap_height(title_[0]) / 2;
    int start_x = WIDTH / 2, start_y = HEIGHT / 4;
    int setting_x = WIDTH / 2, setting_y = start_y + IMAGE_HEIGHT + 30;
    int about_x = WIDTH / 2, about_y = setting_y + IMAGE_HEIGHT + 30;
    int exit_x = WIDTH / 2, exit_y = about_y + IMAGE_HEIGHT + 30;

    if (!title_red) {
        al_draw_bitmap(title_[0], title_x, title_y, 0);
    } else {
        al_draw_bitmap(title_[1], title_x, title_y, 0);
    }
    al_draw_bitmap(start_game[0], start_x, start_y, 0);
    al_draw_bitmap(setting[0], setting_x, setting_y, 0);
    al_draw_bitmap(about[0], about_x, about_y, 0);
    al_draw_bitmap(exit_scene[0], exit_x, exit_y, 0);

    if (mouse_state_.x >= start_x && mouse_state_.x <= start_x + al_get_bitmap_width(start_game[0]) && mouse_state_.y >= start_y && mouse_state_.y <= start_y + IMAGE_HEIGHT) {
        now_on = FIGHT;
        al_draw_bitmap(start_game[1], start_x, start_y, 0);
    } else if (mouse_state_.x >= setting_x && mouse_state_.x <= setting_x + al_get_bitmap_width(setting[0]) && mouse_state_.y >= setting_y && mouse_state_.y <= setting_y + IMAGE_HEIGHT) {
        now_on = SETTING;
        al_draw_bitmap(setting[1], setting_x, setting_y, 0);
    } else if (mouse_state_.x >= about_x && mouse_state_.x <= about_x + al_get_bitmap_width(about[0]) && mouse_state_.y >= about_y && mouse_state_.y <= about_y + IMAGE_HEIGHT) {
        now_on = ABOUT;
        al_draw_bitmap(about[1], about_x, about_y, 0);
    } else if (mouse_state_.x >= exit_x && mouse_state_.x <= exit_x + al_get_bitmap_width(exit_scene[0]) && mouse_state_.y >= exit_y && mouse_state_.y <= exit_y + IMAGE_HEIGHT) {
        now_on = EXIT;
        al_draw_bitmap(exit_scene[1], exit_x, exit_y, 0);
    } else if (mouse_state_.x >= title_x && mouse_state_.x <= title_x + al_get_bitmap_width(title_[0]) && mouse_state_.y >= title_y && mouse_state_.y <= title_y + al_get_bitmap_height(title_[0])) {
        now_on = TITLE_RED;
        al_draw_bitmap(title_[1], title_x, title_y, 0);
    } else {
        now_on = -1;
    }

    // al_clear_to_color(al_map_rgb(100, 100, 100));
    // al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2 + 220, ALLEGRO_ALIGN_CENTRE, "Press 'Enter' to start");
    // al_draw_rectangle(WIDTH / 2 - 150, 510, WIDTH / 2 + 150, 550, al_map_rgb(255, 255, 255), 0);
}
void menu_destroy() {
    al_destroy_font(font);
    al_destroy_bitmap(background);
    al_destroy_bitmap(start_game[0]);
    al_destroy_bitmap(start_game[1]);
    al_destroy_bitmap(setting[0]);
    al_destroy_bitmap(setting[1]);
    al_destroy_bitmap(about[0]);
    al_destroy_bitmap(about[1]);
    al_destroy_bitmap(exit_scene[0]);
    al_destroy_bitmap(exit_scene[1]);
}

// function of game_scene
void game_scene1_init() {
    font = al_load_ttf_font("./font/源樣明體-超粗.ttc", 24, 0);
    // XXX: object init
    character_init();
    monster_init();
    pause_init();
    camera_update();
    chief_init();
    chat_init();
    // background = al_load_bitmap("./image/stage.jpg");
    create_scene_map_img("./map/scene1.txt", "./map/scene1.jpg");
    scene1_img = al_load_bitmap("./map/scene1.jpg");
}
void game_scene1_draw() {
    // printf("x%d y%d\n", OBJECT_MODIFY_X, OBJECT_MODIFY_Y);
    // scanf("%d", OBJECT_MODIFY_X);

    if (check_end()) {
        after_end();
    }

    if (winner == -1) {
        al_clear_to_color(al_map_rgb(255, 255, 255));
    } else {
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }
    al_draw_bitmap(scene1_img, OBJECT_MODIFY_X, OBJECT_MODIFY_Y, 0);
    // XXX: object draw
    character_draw();
    bullets_draw();
    monster_draw();
    objects_draw();
    chief_of_village_draw();

    if (show_bar) bar_draw();
    if (pause) pause_draw();
    if (in_chat) chat_draw();

    // if (check_end()) {
    //     printf("===============\n");
    //     black_scene = true;
    //     if (black_scene) {
    //         ALLEGRO_BITMAP* a;
    //         a = al_load_bitmap("./image/black.png");
    //         al_draw_bitmap(a, 0, 0, 0);
    //         printf("jifahsilfuhaslidjglsajgn\n");
    //         after_end();
    //         black_scene = false;
    //     }
    // }
}
void game_scene1_destroy() {
    al_destroy_bitmap(scene1_img);
    // XXX: object destory
    character_destory();
    bullets_destory();
    monster_destory();
    chat_destory();
}

// boss mode
enum { RANDOM_MODE, QL_MODE };
ALLEGRO_BITMAP* setting_boss;
ALLEGRO_BITMAP* random[2];
ALLEGRO_BITMAP* ql[2];
void setting_scene_init() {
    setting_boss = al_load_bitmap("./image/setting_boss.png");
    random[0] = al_load_bitmap("./image/random_0.png");
    random[1] = al_load_bitmap("./image/random_1.png");
    ql[0] = al_load_bitmap("./image/ql_0.png");
    ql[1] = al_load_bitmap("./image/ql_1.png");
}
void setting_scene_process(ALLEGRO_EVENT event) {
    if (event.type == ALLEGRO_EVENT_KEY_UP) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            aaa_scene = MENU;
            last_scene = ABOUT;
            judge_next_window = true;
        }
    }

    al_get_mouse_state(&mouse_state_);
    if (mouse_state_.buttons & 1 && now_on == RANDOM_MODE) {
        boss_mode = RANDOM_MODE;
    } else if (mouse_state_.buttons & 1 && now_on == QL_MODE) {
        boss_mode = QL_MODE;
    }
}
void setting_scene_draw() {
    al_draw_bitmap(background, 0, 0, 0);

    int setting_boss_x = 60, setting_boss_y = 60;
    int random_x = setting_boss_x + 500, random_y = setting_boss_y;
    int ql_x = random_x + 500, ql_y = setting_boss_y;

    al_draw_bitmap(setting_boss, setting_boss_x, setting_boss_y, 0);

    if (boss_mode == RANDOM_MODE) {
        al_draw_bitmap(random[1], random_x, random_y, 0);
        al_draw_bitmap(ql[0], ql_x, ql_y, 0);
    } else if (boss_mode == QL_MODE) {
        al_draw_bitmap(random[0], random_x, random_y, 0);
        al_draw_bitmap(ql[1], ql_x, ql_y, 0);
    }

    if (mouse_state_.x >= random_x && mouse_state_.x <= random_x + al_get_bitmap_width(random[0]) && mouse_state_.y >= random_y && mouse_state_.y <= random_y + al_get_bitmap_height(random[0])) {
        now_on = RANDOM_MODE;
        al_draw_bitmap(random[1], random_x, random_y, 0);
    } else if (mouse_state_.x >= ql_x && mouse_state_.x <= ql_x + al_get_bitmap_width(ql[0]) && mouse_state_.y >= ql_y && mouse_state_.y <= ql_y + al_get_bitmap_height(ql[0])) {
        now_on = QL_MODE;
        al_draw_bitmap(ql[1], ql_x, ql_y, 0);
    }
}
void setting_scene_destory() {
    al_destroy_bitmap(setting_boss);
    al_destroy_bitmap(random[0]);
    al_destroy_bitmap(random[1]);
    al_destroy_bitmap(ql[0]);
    al_destroy_bitmap(ql[1]);
}

ALLEGRO_BITMAP* about_page;
void about_scene_init() {
    about_page = al_load_bitmap("./image/about.png");
}
void about_scene_process(ALLEGRO_EVENT event) {
    if (event.type == ALLEGRO_EVENT_KEY_UP) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            aaa_scene = MENU;
            last_scene = ABOUT;
            judge_next_window = true;
        }
    }
}
void about_scene_draw() {
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_bitmap(about_page, (WIDTH / 2) - (al_get_bitmap_width(about_page) / 2), (HEIGHT / 2) - (al_get_bitmap_height(about_page) / 2), 0);
}
void about_scene_destroy() {
    al_destroy_bitmap(about_page);
}


ALLEGRO_BITMAP* congrate;
ALLEGRO_BITMAP* game_over;
void exit_fighting_scene_init() {
    congrate = al_load_bitmap("./image/congrates.png");
    game_over = al_load_bitmap("./image/game_over.png");
}
void exit_fighting_scene_process(ALLEGRO_EVENT event) {
    if (event.type == ALLEGRO_EVENT_KEY_UP) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            aaa_scene = MENU;
            last_scene = EXIT_PAGE;
            judge_next_window = true;

            title_red = true;

            in_chat = false;
            in_setting = false;
            pause = false;
            terminate = false;
            show_bar = false;
            black_scene = false;
            winner = -1;
            chat_count = -1;
        }
    }
}
void exit_fighting_scene_draw() {
    al_draw_bitmap(background, 0, 0, 0);
    if (winner == 666) {
        al_draw_bitmap(congrate, WIDTH / 2 - al_get_bitmap_width(congrate) / 2, HEIGHT / 2 - al_get_bitmap_height(congrate) / 2, 0);
    } else if (winner == 444) {
        al_draw_bitmap(game_over, WIDTH / 2 - al_get_bitmap_width(game_over) / 2, HEIGHT / 2 - al_get_bitmap_height(game_over) / 2, 0);
    }
}
void exit_fighting_scene_destory() {
    al_destroy_bitmap(congrate);
    al_destroy_bitmap(game_over);
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
            } else if (block_num == 5) {
                sprintf(scene_block_path, "./image/%d.png", 5);
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
