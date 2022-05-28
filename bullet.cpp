#define MAX_BULLET_NUM 100

#include "bullet.h"

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
