#define MAX_BULLET_NUM 100

#include "charater.h"

/////////////// 注意事項 ///////////////
// 1: object 的 x, y 都是指該物件的左上角
// 2: 物件要繪出，都是加上 OBJECT_MODIFY
// 3: object init coor. must + width/height.
///////////////////////////////////////

// the state of character
enum { STOP = 0, MOVE, ATK };
typedef enum direction { UP, DOWN, RIGHT, LEFT } Direction;
typedef struct character {
    int x, y;             // the position of image
    int width, height;    // the width and height of image
    bool dir;             // left: false, right: true
    int face_dir;         // enum direction
    int state;            // the state of character
    bool under_atk_cd;    // under attack cool down
    ALLEGRO_BITMAP* img_move[2];
    ALLEGRO_BITMAP* img_atk[2];
    ALLEGRO_SAMPLE_INSTANCE* atk_Sound;
    int anime;         // counting the time of animation
    int anime_time;    // indicate how long the animation
} Character;

Character chara;
ALLEGRO_SAMPLE* sample = NULL;
void character_init() {
    // load character images
    for (int i = 1; i <= 2; i++) {
        char temp[50];
        sprintf(temp, "./image/char_move%d.png", i);
        chara.img_move[i - 1] = al_load_bitmap(temp);
    }
    for (int i = 1; i <= 2; i++) {
        char temp[50];
        sprintf(temp, "./image/char_atk%d.png", i);
        chara.img_atk[i - 1] = al_load_bitmap(temp);
    }
    // load effective sound
    sample = al_load_sample("./sound/atk_sound.wav");
    chara.atk_Sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(chara.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(chara.atk_Sound, al_get_default_mixer());

    // initial the geometric information of character
    chara.width = al_get_bitmap_width(chara.img_move[0]);
    chara.height = al_get_bitmap_height(chara.img_move[0]);
    chara.x = 50;
    chara.y = 400;
    chara.dir = true;

    // initial gaming info of character
    chara.face_dir = RIGHT;
    chara.under_atk_cd = false;

    // initial the animation component
    chara.state = STOP;
    chara.anime = 0;
    chara.anime_time = 30;
}
void character_process(ALLEGRO_EVENT event) {
    // process the animation
    if (event.type == ALLEGRO_EVENT_TIMER) {
        if (event.timer.source == fps) {
            chara.anime++;
            chara.anime %= chara.anime_time;
        }
        // process the keyboard event
    } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        key_state[event.keyboard.keycode] = true;
        chara.anime = 0;
    } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
        key_state[event.keyboard.keycode] = false;
    }
}
void character_update() {
    // use the idea of finite state machine to deal with different state
    if (key_state[ALLEGRO_KEY_W]) {
        if (!chara_is_collide_object()) chara.y -= 5;
        chara.face_dir = UP;
        chara.state = MOVE;
    } else if (key_state[ALLEGRO_KEY_A]) {
        chara.dir = false;
        if (!chara_is_collide_object()) chara.x -= 5;
        chara.face_dir = LEFT;
        chara.state = MOVE;
    } else if (key_state[ALLEGRO_KEY_S]) {
        if (!chara_is_collide_object()) chara.y += 5;
        chara.face_dir = DOWN;
        chara.state = MOVE;
    } else if (key_state[ALLEGRO_KEY_D]) {
        chara.dir = true;
        if (!chara_is_collide_object()) chara.x += 5;
        chara.face_dir = RIGHT;
        chara.state = MOVE;
    } else if (key_state[ALLEGRO_KEY_SPACE]) {
        chara.state = ATK;
    } else if (chara.anime == chara.anime_time - 1) {
        chara.anime = 0;
        chara.state = STOP;
    } else if (chara.anime == 0) {
        chara.state = STOP;
    } else if (key_state[ALLEGRO_KEY_R]) {
        chara.x = 500;
        chara.y = 500;
    }
}
void character_draw() {
    // printf("chara_x=%d chara_y=%d\n", chara.x, chara.y);
    // with the state, draw corresponding image
    int modified_x = (chara.x) + OBJECT_MODIFY_X;
    int modified_y = (chara.y) + OBJECT_MODIFY_Y;

    if (chara.state == STOP) {
        if (chara.dir) al_draw_bitmap(chara.img_move[0], modified_x, modified_y, ALLEGRO_FLIP_HORIZONTAL);
        else al_draw_bitmap(chara.img_move[0], modified_x, modified_y, 0);
    } else if (chara.state == MOVE) {
        if (chara.dir) {
            if (chara.anime < chara.anime_time / 2) {
                al_draw_bitmap(chara.img_move[0], modified_x, modified_y, ALLEGRO_FLIP_HORIZONTAL);
            } else {
                al_draw_bitmap(chara.img_move[1], modified_x, modified_y, ALLEGRO_FLIP_HORIZONTAL);
            }
        } else {
            if (chara.anime < chara.anime_time / 2) {
                al_draw_bitmap(chara.img_move[0], modified_x, modified_y, 0);
            } else {
                al_draw_bitmap(chara.img_move[1], modified_x, modified_y, 0);
            }
        }
    } else if (chara.state == ATK) {
        if (chara.dir) {
            if (chara.anime < chara.anime_time / 2) {
                chara.under_atk_cd = false;
                al_draw_bitmap(chara.img_atk[0], modified_x, modified_y, ALLEGRO_FLIP_HORIZONTAL);
            } else {
                al_draw_bitmap(chara.img_atk[1], modified_x, modified_y, ALLEGRO_FLIP_HORIZONTAL);
                al_play_sample_instance(chara.atk_Sound);
                // 避免重複攻擊
                if (!chara.under_atk_cd) {
                    chara.under_atk_cd = true;
                    bullet_init(5);
                }
            }
        } else {
            if (chara.anime < chara.anime_time / 2) {
                chara.under_atk_cd = false;
                al_draw_bitmap(chara.img_atk[0], modified_x, modified_y, 0);
            } else {
                al_draw_bitmap(chara.img_atk[1], modified_x, modified_y, 0);
                al_play_sample_instance(chara.atk_Sound);
                // 避免重複攻擊
                if (!chara.under_atk_cd) {
                    chara.under_atk_cd = true;
                    bullet_init(5);
                }
            }
        }
    }
}
void character_destory() {
    al_destroy_bitmap(chara.img_atk[0]);
    al_destroy_bitmap(chara.img_atk[1]);
    al_destroy_bitmap(chara.img_move[0]);
    al_destroy_bitmap(chara.img_move[1]);
    al_destroy_sample_instance(chara.atk_Sound);
}

// the state of bullet
enum { FLY, INIT };
typedef struct bullet {
    int x, y;
    int fly_dir_x = 0, fly_dir_y = 0;
    int speed;
    int width, height;
    int state = INIT;
    ALLEGRO_BITMAP* img;
} Bullet;

Bullet bullets[MAX_BULLET_NUM + 1];
int bullets_start_index = 0;
int bullets_end_index = 0;
void bullet_init(int speed) {
    // load bullet image
    bullets[bullets_end_index].img = al_load_bitmap("./image/bullet.png");

    // initial the geometric information of bullet
    bullets[bullets_end_index].width = al_get_bitmap_width(bullets[bullets_end_index].img);
    bullets[bullets_end_index].height = al_get_bitmap_height(bullets[bullets_end_index].img);

    // initial the animation component
    bullets[bullets_end_index].state = FLY;
    bullets[bullets_end_index].speed = speed;
    if (chara.face_dir == UP) {
        bullets[bullets_end_index].fly_dir_y = -1;
    } else if (chara.face_dir == DOWN) {
        bullets[bullets_end_index].fly_dir_y = 1;
    } else if (chara.face_dir == LEFT) {
        bullets[bullets_end_index].fly_dir_x = -1;
    } else if (chara.face_dir == RIGHT) {
        bullets[bullets_end_index].fly_dir_x = 1;
    }

    // initial the initial x, y
    bullets[bullets_end_index].x = chara.x + chara.width / 2 - bullets[bullets_end_index].width / 2;
    bullets[bullets_end_index].y = chara.y + chara.height / 2 - bullets[bullets_end_index].height / 2;

    bullets_end_index = (bullets_end_index + 1) % MAX_BULLET_NUM;
}
void bullets_update() {
    // debug
    // printf("start_index=%5d end_index=%5d\n", bullets_start_index, bullets_end_index);
    // end debug
    // 調整bullets_start_index
    for (int i = bullets_start_index; i != bullets_end_index; i = (i + 1) % MAX_BULLET_NUM) {
        if (bullets[i].state == INIT) {
            bullets_start_index = (bullets_start_index + 1) % MAX_BULLET_NUM;
        } else {
            break;
        }
    }

    // 移動bullets
    for (int i = bullets_start_index; i != bullets_end_index; i = (i + 1) % MAX_BULLET_NUM) {
        if (bullets[i].state == INIT) {
            continue;
        } else if (bullets[i].state == FLY) {
            int bullets_dx = bullets[i].fly_dir_x * bullets[i].speed;
            int bullets_dy = bullets[i].fly_dir_y * bullets[i].speed;
            // 判斷bullet該不該消失
            if (bullets[i].x + bullets_dx >= (BLOCK_WIDTH * THIS_SCENE_SIZE_COL) + (WIDTH / 2) + bullets[i].width || bullets[i].x + bullets_dx <= -bullets[i].width - (WIDTH / 2)) {
                // printf("bye\n");
                Bullet reset_bullet;
                bullets[i] = reset_bullet;
                continue;
            } else if (bullets[i].y + bullets_dy >= (BLOCK_HEIGHT * THIS_SCENE_SIZE_ROW) + (HEIGHT / 2) + bullets[i].height || bullets[i].y + bullets_dy <= -bullets[i].height - (HEIGHT / 2)) {
                // printf("bye\n");
                Bullet reset_bullet;
                bullets[i] = reset_bullet;
                continue;
            } else if (bullet_is_collide_object(i, bullets_dx, bullets_dy)) {
                Bullet reset_bullet;
                bullets[i] = reset_bullet;
                continue;
            }

            bullets[i].x += bullets_dx;
            bullets[i].y += bullets_dy;
        }
    }
}
void bullets_draw() {
    for (int i = bullets_start_index; i != bullets_end_index; i = (i + 1) % MAX_BULLET_NUM) {
        int modified_x = bullets[i].x + OBJECT_MODIFY_X;    // - (bullets[i].width / 2);
        int modified_y = bullets[i].y + OBJECT_MODIFY_Y;    // - (bullets[i].height / 2);

        if (bullets[i].state == INIT) {
            continue;
        } else if (bullets[i].state == FLY) {
            al_draw_bitmap(bullets[i].img, modified_x, modified_y, 0);
        }
    }
}
void bullets_destory() {
    for (int i = 0; i < MAX_BULLET_NUM; i++) {
        if (bullets[i].state == FLY) {
            al_destroy_bitmap(bullets[i].img);
            printf("%d\n", i);
        }
    }
}

// the state of monster
enum { MONS_STOP, MONS_MOVE, MONS_ATTACK1, MONS_ATTACK2, MONS_ATTACK3 };
typedef struct monster {
    int x, y;                      // the position of image
    int width, height;             // the width and height of image
    float lock_dirx, lock_diry;    // lock the player(chara) direction
    int state;                     // the state of character
    bool under_atk_cd;             // under attack cool down
    ALLEGRO_BITMAP* img_move[3];
    ALLEGRO_SAMPLE_INSTANCE* atk_Sound;
    ALLEGRO_SAMPLE_INSTANCE* paji_Sound;
    int anime;         // counting the time of animation
    int anime_time;    // indicate how long the animation
} Monster;

Monster monst;
ALLEGRO_SAMPLE* sample_atk = NULL;
ALLEGRO_SAMPLE* sample_paji = NULL;
void monster_init() {
    // load character images
    for (int i = 1; i <= 3; i++) {
        char temp[50];
        sprintf(temp, "./image/fire_monster_%d.png", i);
        monst.img_move[i - 1] = al_load_bitmap(temp);
    }
    // load effective sound
    // atk sound
    sample_atk = al_load_sample("./sound/monster_atk_sound.wav");
    monst.atk_Sound = al_create_sample_instance(sample_atk);
    al_set_sample_instance_playmode(monst.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(monst.atk_Sound, al_get_default_mixer());
    // paji_sound
    sample_paji = al_load_sample("./sound/monster_paji_sound.wav");
    monst.paji_Sound = al_create_sample_instance(sample_paji);
    al_set_sample_instance_playmode(monst.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(monst.atk_Sound, al_get_default_mixer());

    // initial the geometric information of character
    monst.width = al_get_bitmap_width(monst.img_move[0]);
    monst.height = al_get_bitmap_height(monst.img_move[0]);
    monst.x = (WIDTH / 2) - monst.width;
    monst.y = (HEIGHT / 2) - monst.height;

    // initial gaming info of character
    monst.lock_dirx = 0;    // TODO: change to float
    monst.lock_diry = 0;    // TODO: change to float
    monst.under_atk_cd = false;

    // initial the animation component
    monst.state = MONS_STOP;
    monst.anime = 0;
    monst.anime_time = 60;
}
void monster_process(ALLEGRO_EVENT event) {
    // process the animation
    if (event.type == ALLEGRO_EVENT_TIMER) {
        if (event.timer.source == fps) {
            monst.anime++;
            monst.anime %= monst.anime_time;
        }
    }
}
void monster_update() {
    // use the idea of finite state machine to deal with different state
}
void monster_draw() {
    int modified_x = monst.x + OBJECT_MODIFY_X - (monst.width / 2);
    int modified_y = monst.y + OBJECT_MODIFY_Y - (monst.height / 2);
    // with the state, draw corresponding image
    if (monst.state == STOP) {
        if (monst.anime < monst.anime_time / 2) {
            al_draw_bitmap(monst.img_move[0], modified_x, modified_y, 0);
        } else {
            al_draw_bitmap(monst.img_move[1], modified_x, modified_y, 0);
        }
    }
}
void monster_destory() {
    al_destroy_bitmap(monst.img_move[0]);
    al_destroy_bitmap(monst.img_move[1]);
    al_destroy_bitmap(monst.img_move[2]);
    al_destroy_sample_instance(monst.atk_Sound);
    al_destroy_sample_instance(monst.paji_Sound);
}

int max(int a, int b) {
    return ((a > b) ? a : b);
}
int min(int a, int b) {
    return ((a < b) ? a : b);
}
void objects_draw() {
    ALLEGRO_BITMAP* wall = al_load_bitmap("./image/3.png");
    int draw_start_x = chara.x / BLOCK_WIDTH;
    int draw_start_y = chara.y / BLOCK_HEIGHT;

    for (int i = max(0, draw_start_y - (HEIGHT / BLOCK_HEIGHT) + 1), count_i = 0; (i < THIS_SCENE_SIZE_ROW) && (count_i < ((HEIGHT / BLOCK_HEIGHT) + 10)); i++, count_i++) {
        for (int j = max(0, draw_start_x - (WIDTH / BLOCK_WIDTH) + 1), count_j = 0; (j < THIS_SCENE_SIZE_COL) && (count_j < ((WIDTH / BLOCK_WIDTH) + 10)); j++, count_j++) {
            int modified_x = (j + 1) * BLOCK_WIDTH + OBJECT_MODIFY_X;
            int modified_y = (i + 1) * BLOCK_HEIGHT + OBJECT_MODIFY_Y;

            if (THIS_SCENE_MAP[i][j] == 3) {
                al_draw_bitmap(wall, modified_x - al_get_bitmap_width(wall), modified_y - al_get_bitmap_height(wall), 0);
            }
        }
    }
}

bool chara_is_collide_object() {
    // width 和 height 減掉一為了讓角色能夠通過剛好的縫隙
    int lu_coor_x = chara.x, lu_coor_y = chara.y;                                         // 左上
    int ru_coor_x = chara.x + chara.width - 1, ru_coor_y = chara.y;                       // 右上
    int ld_coor_x = chara.x, ld_coor_y = chara.y + chara.height - 1;                      // 左下
    int rd_coor_x = chara.x + chara.width - 1, rd_coor_y = chara.y + chara.height - 1;    // 右下

    if (key_state[ALLEGRO_KEY_W]) {
        lu_coor_y -= 5;
        ru_coor_y -= 5;
        ld_coor_y -= 5;
        rd_coor_y -= 5;
    }
    if (key_state[ALLEGRO_KEY_S]) {
        lu_coor_y += 5;
        ru_coor_y += 5;
        ld_coor_y += 5;
        rd_coor_y += 5;
    }
    if (key_state[ALLEGRO_KEY_D]) {
        lu_coor_x += 5;
        ru_coor_x += 5;
        ld_coor_x += 5;
        rd_coor_x += 5;
    }
    if (key_state[ALLEGRO_KEY_A]) {
        lu_coor_x -= 5;
        ru_coor_x -= 5;
        ld_coor_x -= 5;
        rd_coor_x -= 5;
    }

    if ((THIS_SCENE_MAP[lu_coor_y / BLOCK_HEIGHT][lu_coor_x / BLOCK_WIDTH] == 3) || (THIS_SCENE_MAP[ru_coor_y / BLOCK_HEIGHT][ru_coor_x / BLOCK_WIDTH] == 3) ||
        (THIS_SCENE_MAP[ld_coor_y / BLOCK_HEIGHT][ld_coor_x / BLOCK_WIDTH] == 3) || (THIS_SCENE_MAP[rd_coor_y / BLOCK_HEIGHT][rd_coor_x / BLOCK_WIDTH] == 3)) {
        return true;
    } else return false;
}
bool bullet_is_collide_object(int i, int dx, int dy) {
    int lu_coor_x = dx + bullets[i].x, lu_coor_y = dy + bullets[i].y;                                           // 左上
    int ru_coor_x = dx + bullets[i].x + bullets[i].width, ru_coor_y = dy + bullets[i].y;                        // 右上
    int ld_coor_x = dx + bullets[i].x, ld_coor_y = dy + bullets[i].y + bullets[i].height;                       // 左下
    int rd_coor_x = dx + bullets[i].x + bullets[i].width, rd_coor_y = dy + bullets[i].y + bullets[i].height;    // 右下

    if ((THIS_SCENE_MAP[lu_coor_y / BLOCK_HEIGHT][lu_coor_x / BLOCK_WIDTH] == 3) || (THIS_SCENE_MAP[ru_coor_y / BLOCK_HEIGHT][ru_coor_x / BLOCK_WIDTH] == 3) ||
        (THIS_SCENE_MAP[ld_coor_y / BLOCK_HEIGHT][ld_coor_x / BLOCK_WIDTH] == 3) || (THIS_SCENE_MAP[rd_coor_y / BLOCK_HEIGHT][rd_coor_x / BLOCK_WIDTH] == 3)) {
        return true;
    } else return false;
}

void camera_update() {
    OBJECT_MODIFY_X = (WIDTH / 2) - (chara.x) - (chara.width / 2);
    OBJECT_MODIFY_Y = (HEIGHT / 2) - (chara.y) - (chara.height / 2);
}
