#define MAX_BULLET_NUM 10000
#define PI acos(-1)

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <windows.h>

#include "charater.h"

/////////////// 注意事項 ///////////////
// 1: object 的 x, y 都是指該物件的左上角
// 2: 物件要繪出，都是加上 OBJECT_MODIFY
// 3: object init coor. must + width/height.
///////////////////////////////////////

bool talkable = false;
int passed_time = 0;

// boss mode
enum { RANDOM_MODE, QL_MODE };
// teams
enum { T_MONST, T_CHARA, T_STATE };
ALLEGRO_MOUSE_STATE mouse_state;

int max(int a, int b) {
    return ((a > b) ? a : b);
}
int min(int a, int b) {
    return ((a < b) ? a : b);
}
double max_arr(double arr[]) {
    double max_ele = 0;
    for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
        max_ele = max(max_ele, arr[i]);
    }
    return max_ele;
}
typedef struct qlearning {
    double learning_rate;
    double discount_factor;
    double epsilon;
    double q_table[30][5];    // x_axis: distance, y_axis: actions

    void learn(int state, int action, int reward, int next_state) {
        double now_q = this->q_table[state][action];
        double new_q = reward + this->discount_factor * max_arr(this->q_table[next_state]);
        this->q_table[state][action] += this->learning_rate * (new_q - now_q);
        // printf("[learned] i=%d k=%d %llf\n", state, action, this->q_table[state][action]);
    };
    int get_action(int state) {
        if (boss_mode == RANDOM_MODE) this->epsilon = 1;
        else if (boss_mode == QL_MODE) this->epsilon = 0.2;

        int action;
        double random = ((double)rand() / (double)(RAND_MAX + 0.01));
        if (random < this->epsilon) {
            action = rand() % 5;
            printf("#random# action=%d rand=%llf\n", action, random);
        } else {
            int index_has_max_ele[5];
            int index_num_count = 0;
            double max_val = 0;
            for (int i = 0; i < 5; i++) {
                if (this->q_table[state][i] > max_val) {
                    max_val = this->q_table[state][i];
                    index_has_max_ele[0] = i;
                    index_num_count = 1;
                } else if (this->q_table[state][i] == max_val) {
                    index_has_max_ele[index_num_count] = i;
                    index_num_count++;
                }
            }

            return index_has_max_ele[rand() % index_num_count];
        }

        return action;
    };
    void save_learning_res() {
        FILE* file;
        file = fopen("./q_learning_res.txt", "w");
        for (int i = 0; i < 30; i++) {
            for (int k = 0; k < 5; k++) {
                fprintf(file, "%llf\n", this->q_table[i][k]);
            }
        }
        fclose(file);
        printf("[fin] saved q learning res.\n");
    };
} QL;

QL agent;
void agent_init() {
    FILE* file;
    file = fopen("./q_learning_res.txt", "r");

    agent.learning_rate = 0.01;
    agent.discount_factor = 0.9;
    agent.epsilon = 1;
    for (int i = 0; i < 30; i++) {
        for (int k = 0; k < 5; k++) {
            fscanf(file, "%llf", &agent.q_table[i][k]);
            // agent.q_table[i][k] = 0;
            // printf("%d %d %d %llf\n", i, j, k, agent.q_table[i][j][k]);
        }
    }

    fclose(file);
}


// the state of character
enum { STOP = 0, MOVE, ATK, BLOCK };
typedef enum direction { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 } Direction;
typedef struct character {
    int hp;
    int MAX_HP;
    bool block;
    int x, y;             // the position of image
    int width, height;    // the width and height of image
    bool dir;             // left: false, right: true
    int face_dir;         // enum direction
    int state;            // the state of character
    bool under_atk_cd;    // under attack cool down
    ALLEGRO_BITMAP* img_move[12];
    ALLEGRO_BITMAP* img_atk[2];
    ALLEGRO_SAMPLE_INSTANCE* atk_Sound;
    int anime;         // counting the time of animation
    int anime_time;    // indicate how long the animation
} Character;

Character chara;
ALLEGRO_SAMPLE* sample = NULL;
void character_init() {
    srand(time(NULL));
    bar_init(1, WIDTH / 2 - 30, HEIGHT / 2 - 40, 60, 5);

    chara.MAX_HP = 100;
    chara.hp = chara.MAX_HP;
    chara.block = false;
    // load character images
    for (int i = 1; i <= 12; i++) {
        char temp[50];
        sprintf(temp, "./image/char_move_%d.png", i);
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
    chara.x = 700;
    chara.y = 2400;
    chara.dir = true;

    // initial gaming info of character
    chara.face_dir = DOWN;
    chara.under_atk_cd = false;

    // initial the animation component
    chara.state = STOP;
    chara.anime = 0;
    chara.anime_time = 30;
}
void character_process(ALLEGRO_EVENT event) {
    // count time to save QL res.
    passed_time++;
    if (passed_time / 1000 > 0) {
        agent.save_learning_res();
        passed_time = 0;
    }
    // process the animation
    if (event.type == ALLEGRO_EVENT_TIMER) {
        // chara anime
        if (event.timer.source == fps) {
            chara.anime++;
            chara.anime %= chara.anime_time;
        }
        // process the keyboard event
    } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        key_state[event.keyboard.keycode] = true;
        chara.anime = 0;
    } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            pause = true;
        } else if (event.keyboard.keycode == ALLEGRO_KEY_R) {
            if (winner != -1 || chat_count == -1) {
                talkable = false;
                if (winner == 444) {
                    chat_count = 11;
                } else if (winner == 666) {
                    chat_count = 5;
                }
                next_part_of_chat();
            }
        }
        key_state[event.keyboard.keycode] = false;
    }
    al_get_mouse_state(&mouse_state);
}
void character_update() {
    if (chara.state == BLOCK) return;
    if (chara_is_collide_portal()) {
        reset_positions(750, 1400, 700, 700);
        show_bar = true;
        monst_go();
        return;
    }
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
    } else if (chara.anime == chara.anime_time - 1) {
        chara.anime = 0;
        chara.state = STOP;
    } else if (chara.anime == 0) {
        chara.state = STOP;
    }

    // else if (key_state[ALLEGRO_KEY_R]) {
    //     // chara.x = 500;
    //     // chara.y = 500;
    //     for (int i = 0; i < 30; i++) {
    //         for (int k = 0; k < 5; k++) {
    //             printf("%llf\n", agent.q_table[i][k]);
    //         }
    //         printf("\n");
    //     }
    //     al_stop_timer(fps);
    //     Sleep(2000);
    //     // int a;
    //     // scanf("%d", &a);
    //     // printf("\n\n");
    //     al_resume_timer(fps);
    // }
    if (mouse_state.buttons & 1) {    // mouse left button down
        chara.state = ATK;
        // printf("Mouse position: (%d, %d)\n", state.x, state.y);
    }
}
void character_draw() {
    // printf("chara_x=%d chara_y=%d\n", chara.x, chara.y);
    // with the state, draw corresponding image
    int modified_x = (chara.x) + OBJECT_MODIFY_X;
    int modified_y = (chara.y) + OBJECT_MODIFY_Y;

    if (chara.state == STOP) {
        chara.block = false;
        al_draw_bitmap(chara.img_move[3 * chara.face_dir], modified_x, modified_y, 0);
    } else if (chara.state == MOVE) {
        if (chara.anime < chara.anime_time / 2) {
            al_draw_bitmap(chara.img_move[3 * chara.face_dir + 1], modified_x, modified_y, 0);
        } else {
            al_draw_bitmap(chara.img_move[3 * chara.face_dir + 2], modified_x, modified_y, 0);
        }
    } else if (chara.state == ATK) {
        al_draw_bitmap(chara.img_move[3 * chara.face_dir], modified_x, modified_y, 0);
        if (chara.anime >= chara.anime_time / 6) {
            // al_play_sample_instance(chara.atk_Sound);
            if (chara.anime % 3 == 0) {
                // chara.block = true;
                double rand_dir_x = ((double)rand() / (double)(RAND_MAX + 0.001));
                double rand_dir_y = ((double)rand() / (double)(RAND_MAX + 0.001));
                // printf("shift dx=%f dy=%f\n", rand_dir_x, rand_dir_y);

                bullet_init(T_CHARA, 10, rand_dir_x, rand_dir_y, -1, -1);
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


// the state of monster
enum { MONS_STOP, MONS_MOVE, MONS_ATTACK1, MONS_ATTACK2, MONS_ATTACK3, MONS_BLOCK = 9 };
typedef struct monster {
    int hp;
    int MAX_HP;
    int x, y;                      // the position of image
    int width, height;             // the width and height of image
    float lock_dirx, lock_diry;    // lock the player(chara) direction
    int state;                     // the state of character
    bool under_atk_cd;             // under attack cool down
    ALLEGRO_BITMAP* img_move[3];
    ALLEGRO_SAMPLE_INSTANCE* atk_Sound;
    ALLEGRO_SAMPLE_INSTANCE* paji_Sound;
    int anime;             // counting the time of animation
    int anime_time;        // indicate how long the animation
    int time_for_state;    // counting the time for state
} Monster;

Monster monst;
int delta_times = 0;
ALLEGRO_SAMPLE* sample_atk = NULL;
ALLEGRO_SAMPLE* sample_paji = NULL;
void monster_init() {
    agent_init();
    bar_init(0, WIDTH / 6, HEIGHT / 12, (WIDTH / 3) * 2, 50);

    monst.MAX_HP = 200;
    monst.hp = monst.MAX_HP;
    monst.time_for_state = 0;
    // load character images
    for (int i = 1; i <= 3; i++) {
        char temp[50];
        sprintf(temp, "./image/fire_monster_%d.png", i);
        monst.img_move[i - 1] = al_load_bitmap(temp);
    }
    // load effective sound
    // atk sound
    // sample_atk = al_load_sample("./sound/monster_atk_sound.wav");
    monst.atk_Sound = al_create_sample_instance(sample_atk);
    al_set_sample_instance_playmode(monst.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(monst.atk_Sound, al_get_default_mixer());
    // paji_sound
    // sample_paji = al_load_sample("./sound/monster_paji_sound.wav");
    monst.paji_Sound = al_create_sample_instance(sample_paji);
    al_set_sample_instance_playmode(monst.atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(monst.atk_Sound, al_get_default_mixer());

    // initial the geometric information of character
    monst.width = al_get_bitmap_width(monst.img_move[0]);
    monst.height = al_get_bitmap_height(monst.img_move[0]);
    monst.x = 700;
    monst.y = 700;

    // initial gaming info of character
    monst.lock_dirx = 0;    // TODO: change to float
    monst.lock_diry = 0;    // TODO: change to float
    monst.under_atk_cd = false;

    // initial the animation component
    monst.state = MONS_BLOCK;
    monst.anime = 0;
    monst.anime_time = 60;
}
void monster_process(ALLEGRO_EVENT event) {
    // process the animation
    if (event.type == ALLEGRO_EVENT_TIMER) {
        if (event.timer.source == fps) {
            monst.time_for_state++;
            monst.anime++;
            monst.anime %= monst.anime_time;
        }
    }
}
void generate_next_state() {
    // int min = 0;
    // int max = 5;
    // monst.state = rand() % (max - min + 1);
    // printf("STATE %d\n", monst.state);
    monst.state = agent.get_action(monst_chara_dist());
}
void monster_update() {
    // printf("state=%d\n", monst.state);
    if (monst.state == MONS_BLOCK) return;

    // printf("#epsilon=%llf bossmode=%d\n", agent.epsilon, boss_mode);

    // use the idea of finite state machine to deal with different state
    if (monst.state == MONS_STOP) {
        if (monst.time_for_state >= 60) {
            generate_next_state();
            // monst.state = MONS_MOVE;
            monst.time_for_state = 0;
            bullet_init(T_STATE, 0, 0, 0, 0, monst_chara_dist());
        }
    } else if (monst.state == MONS_MOVE || monst.state > 5) {
        if (monst.time_for_state >= 140) {
            generate_next_state();
            // monst.state = MONS_ATTACK1;
            monst.time_for_state = 0;
        } else if (monst.time_for_state % 70 <= 30) {
            // int distance = sqrt(pow(chara.x - monst.x, 2) + pow(chara.y - monst.y, 2));
            int dx = (chara.x - monst.x) / 150;
            int dy = (chara.y - monst.y) / 150;
            if (!monst_is_collide_object_x(dx)) monst.x += dx;
            if (!monst_is_collide_object_y(dy)) monst.y += dy;
            // printf("dx=%d dy=%d\n", dx, dy);
            bullet_init(T_STATE, 0, 0, 0, 1, sqrt(pow(fabs(monst.x - chara.x), 2) + pow(fabs(monst.y - chara.y), 2)));
        }
    } else if (monst.state == MONS_ATTACK1) {
        int atk_speed = ((monst.hp > monst.MAX_HP / 2) ? 20 : 15);
        int bul_speed = ((monst.hp > monst.MAX_HP / 2) ? 7 : 10);
        if (monst.time_for_state >= 60) {
            generate_next_state();
            // monst.state = MONS_ATTACK2;
            monst.time_for_state = 0;
        }
        if (monst.time_for_state % atk_speed == 0) {
            double rand_dir_x = ((double)rand() / (double)(((RAND_MAX / 0.05) + 0.001)));
            double rand_dir_y = ((double)rand() / (double)(((RAND_MAX / 0.05) + 0.001)));
            double unit_angle = 0.3926990817;
            double delta_angle = atan(((chara.y) - (monst.y)) / ((chara.x) - (monst.x) + 0.01)) + atan(rand_dir_y / rand_dir_x);
            for (int i = 0; i < 16; i++) {
                bullet_init(T_MONST, bul_speed, cos(unit_angle * i + delta_angle), sin(unit_angle * i + delta_angle), 2, monst_chara_dist());
                // printf("angles=%llf cos=%llf sin=%llf\n", unit_angle * i + delta_angle, cos(unit_angle * i + delta_angle), sin(unit_angle * i + delta_angle));
            }
        }
    } else if (monst.state == MONS_ATTACK2) {
        int atk_speed = ((monst.hp > monst.MAX_HP / 2) ? 11 : 7);
        int bul_speed = ((monst.hp > monst.MAX_HP / 2) ? 7 : 10);
        if (monst.time_for_state >= 60) {
            generate_next_state();
            // monst.state = MONS_ATTACK3;
            monst.time_for_state = 0;
            delta_times = 0;
        }
        if (monst.time_for_state % atk_speed == 0) {
            double unit_angle = PI / 2;
            double delta_angle = (PI / 100) * delta_times;
            for (int i = 0; i < 4; i++) {
                bullet_init(T_MONST, bul_speed, cos(unit_angle * i + delta_angle), sin(unit_angle * i + delta_angle), 3, monst_chara_dist());
                delta_times++;
            }
        }
    } else if (monst.state == MONS_ATTACK3) {
        int atk_speed = ((monst.hp > monst.MAX_HP / 2) ? 11 : 7);
        int bul_speed = ((monst.hp > monst.MAX_HP / 2) ? 7 : 10);
        if (monst.time_for_state >= 60) {
            generate_next_state();
            // monst.state = MONS_STOP;
            monst.time_for_state = 0;
        }
        if (monst.time_for_state % atk_speed == 0) {
            double unit_angle = PI / 2;
            double delta_angle = -(PI / 100) * delta_times;
            for (int i = 0; i < 4; i++) {
                bullet_init(T_MONST, bul_speed, cos(unit_angle * i + delta_angle), sin(unit_angle * i + delta_angle), 4, monst_chara_dist());
                delta_times++;
            }
        }
    }
}
void monster_draw() {
    int modified_x = monst.x + OBJECT_MODIFY_X;
    int modified_y = monst.y + OBJECT_MODIFY_Y;
    // with the state, draw corresponding image
    if (monst.anime < monst.anime_time / 2) {
        al_draw_bitmap(monst.img_move[0], modified_x, modified_y, 0);
    } else {
        al_draw_bitmap(monst.img_move[1], modified_x, modified_y, 0);
    }
}
void monster_destory() {
    al_destroy_bitmap(monst.img_move[0]);
    al_destroy_bitmap(monst.img_move[1]);
    al_destroy_bitmap(monst.img_move[2]);
    al_destroy_sample_instance(monst.atk_Sound);
    al_destroy_sample_instance(monst.paji_Sound);
}
void monst_go() {
    monst.state = MONS_STOP;
}

ALLEGRO_BITMAP* chief_of_village;
void chief_init() {
    chief_of_village = al_load_bitmap("./image/90.png");
}
void chief_of_village_draw() {
    int modified_x = 710 + OBJECT_MODIFY_X;
    int modified_y = 2200 + OBJECT_MODIFY_Y;
    al_draw_bitmap(chief_of_village, modified_x, modified_y, 0);
}

// the state of bullet
enum { FLY, INIT };
typedef struct bullet {
    int team;
    int x, y;
    double fly_dir_x = 0, fly_dir_y = 0;
    int speed;
    int width, height;
    int state = INIT;
    ALLEGRO_BITMAP* img;

    // q learning
    int obtain_action;
    int init_monst_state_dist;
} Bullet;

Bullet bullets[MAX_BULLET_NUM + 1];
int bullets_start_index = 0;
int bullets_end_index = 0;
void bullet_init(int team, int speed, float dx, float dy, int obtain_action, int init_monst_state_dist) {    // dx, dy 飄移多少
    // printf("start=%d end=%d\n", bullets_start_index, bullets_end_index);
    // set bullets team
    bullets[bullets_end_index].team = team;

    if (bullets[bullets_end_index].team != T_STATE) {
        // load bullet image
        char temp[50];
        sprintf(temp, "./image/bullet_%d.png", team);
        bullets[bullets_end_index].img = al_load_bitmap(temp);


        // initial the geometric information of bullet
        bullets[bullets_end_index].width = al_get_bitmap_width(bullets[bullets_end_index].img);
        bullets[bullets_end_index].height = al_get_bitmap_height(bullets[bullets_end_index].img);

        // initial the animation component
        bullets[bullets_end_index].state = FLY;
        bullets[bullets_end_index].speed = speed;
        if (bullets[bullets_end_index].team == T_CHARA) {
            bullets[bullets_end_index].fly_dir_x = (mouse_state.x - WIDTH / 2);
            bullets[bullets_end_index].fly_dir_y = (mouse_state.y - HEIGHT / 2) + dy * 100;
        } else if (bullets[bullets_end_index].team == T_MONST) {
            bullets[bullets_end_index].fly_dir_x = dx;
            bullets[bullets_end_index].fly_dir_y = dy;
        }

        double avg = sqrtf(powf(bullets[bullets_end_index].fly_dir_x, 2) + powf(bullets[bullets_end_index].fly_dir_y, 2));
        if (fabs(avg) < 0.1) avg = 0.5;
        bullets[bullets_end_index].fly_dir_x /= avg;
        bullets[bullets_end_index].fly_dir_y /= avg;

        // initial the initial x, y
        if (bullets[bullets_end_index].team == T_CHARA) {
            bullets[bullets_end_index].x = chara.x + chara.width / 2 - bullets[bullets_end_index].width / 2;
            bullets[bullets_end_index].y = chara.y + chara.height / 2 - bullets[bullets_end_index].height / 2;
        } else if (bullets[bullets_end_index].team == T_MONST) {
            bullets[bullets_end_index].x = monst.x + monst.width / 2 - bullets[bullets_end_index].width / 2;
            bullets[bullets_end_index].y = monst.y + monst.height / 2 - bullets[bullets_end_index].height / 2;
        }
    }

    // q learning
    bullets[bullets_end_index].obtain_action = obtain_action;
    bullets[bullets_end_index].init_monst_state_dist = init_monst_state_dist;
    /////////////
    bullets_end_index = (bullets_end_index + 1) % MAX_BULLET_NUM;
}
void bullets_update() {
    // debug
    // printf("start_index=%5d end_index=%5d\n", bullets_start_index, bullets_end_index);
    // end debug
    // 調整bullets_start_index
    for (int i = bullets_start_index; i != bullets_end_index; i = (i + 1) % MAX_BULLET_NUM) {
        if (bullets[i].team == T_STATE) {
            if (agent.q_table[bullets[i].init_monst_state_dist][1] == max_arr(agent.q_table[bullets[i].init_monst_state_dist])) {
                agent.learn(bullets[i].init_monst_state_dist, bullets[i].obtain_action, -3, bullets[i].init_monst_state_dist);
            } else {
                agent.learn(bullets[i].init_monst_state_dist, bullets[i].obtain_action, 3, bullets[i].init_monst_state_dist);
            }
            Bullet reset_bullet;
            bullets[i] = reset_bullet;
            // printf("chara hp=%d\n", chara.hp);
        } else if (bullets[i].state == INIT) {
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
                agent.learn(bullets[i].init_monst_state_dist, bullets[i].obtain_action, -0.01, bullets[i + 1].init_monst_state_dist);
                Bullet reset_bullet;
                bullets[i] = reset_bullet;
                continue;
            } else if (bullets[i].team == T_CHARA && bullet_is_collide_monster(i, bullets_dx, bullets_dy)) {
                Bullet reset_bullet;
                bullets[i] = reset_bullet;
                monst.hp -= 4;
                // printf("monst hp=%d\n", monst.hp);
                continue;
            } else if (bullets[i].team == T_MONST && bullet_is_collide_chara(i, bullets_dx, bullets_dy)) {
                agent.learn(bullets[i].init_monst_state_dist, bullets[i].obtain_action, 200, bullets[i + 1].init_monst_state_dist);
                Bullet reset_bullet;
                bullets[i] = reset_bullet;
                chara.hp -= 4;
                // printf("chara hp=%d\n", chara.hp);
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
            printf("destory bullet: %d\n", i);
        }
    }
    pause_destory();
    setting_scene_destory_chara();
}

ALLEGRO_BITMAP* wall;
void objects_init() {
    wall = al_load_bitmap("./image/3.png");
}
void objects_draw() {
    int draw_start_x = chara.x / BLOCK_WIDTH;
    int draw_start_y = chara.y / BLOCK_HEIGHT;

    for (int i = max(0, draw_start_y - (HEIGHT / BLOCK_HEIGHT) + 1), count_i = 0; (i < THIS_SCENE_SIZE_ROW) && (count_i < ((HEIGHT / BLOCK_HEIGHT) + 20)); i++, count_i++) {
        for (int j = max(0, draw_start_x - (WIDTH / BLOCK_WIDTH) + 1), count_j = 0; (j < THIS_SCENE_SIZE_COL) && (count_j < ((WIDTH / BLOCK_WIDTH) + 20)); j++, count_j++) {
            int modified_x = (j + 1) * BLOCK_WIDTH + OBJECT_MODIFY_X;
            int modified_y = (i + 1) * BLOCK_HEIGHT + OBJECT_MODIFY_Y;

            if (THIS_SCENE_MAP[i][j] == 3) {
                al_draw_bitmap(wall, modified_x - al_get_bitmap_width(wall), modified_y - al_get_bitmap_height(wall), 0);
            }
        }
    }
}

bool chara_is_collide_portal() {
    if (chat_count <= 3) return false;
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

    if ((THIS_SCENE_MAP[lu_coor_y / BLOCK_HEIGHT][lu_coor_x / BLOCK_WIDTH] == 5) || (THIS_SCENE_MAP[ru_coor_y / BLOCK_HEIGHT][ru_coor_x / BLOCK_WIDTH] == 5) ||
        (THIS_SCENE_MAP[ld_coor_y / BLOCK_HEIGHT][ld_coor_x / BLOCK_WIDTH] == 5) || (THIS_SCENE_MAP[rd_coor_y / BLOCK_HEIGHT][rd_coor_x / BLOCK_WIDTH] == 5)) {
        return true;
    } else return false;
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
bool monst_is_collide_object_x(int dx) {
    // width 和 height 減掉一為了讓角色能夠通過剛好的縫隙
    int lu_coor_x = monst.x, lu_coor_y = monst.y;                                         // 左上
    int ru_coor_x = monst.x + monst.width - 1, ru_coor_y = monst.y;                       // 右上
    int ld_coor_x = monst.x, ld_coor_y = monst.y + monst.height - 1;                      // 左下
    int rd_coor_x = monst.x + monst.width - 1, rd_coor_y = monst.y + monst.height - 1;    // 右下

    lu_coor_x += dx;
    ru_coor_x += dx;
    ld_coor_x += dx;
    rd_coor_x += dx;

    if ((THIS_SCENE_MAP[lu_coor_y / BLOCK_HEIGHT][lu_coor_x / BLOCK_WIDTH] == 3) || (THIS_SCENE_MAP[ru_coor_y / BLOCK_HEIGHT][ru_coor_x / BLOCK_WIDTH] == 3) ||
        (THIS_SCENE_MAP[ld_coor_y / BLOCK_HEIGHT][ld_coor_x / BLOCK_WIDTH] == 3) || (THIS_SCENE_MAP[rd_coor_y / BLOCK_HEIGHT][rd_coor_x / BLOCK_WIDTH] == 3)) {
        return true;
    } else return false;
}
bool monst_is_collide_object_y(int dy) {
    // width 和 height 減掉一為了讓角色能夠通過剛好的縫隙
    int lu_coor_x = monst.x, lu_coor_y = monst.y;                                         // 左上
    int ru_coor_x = monst.x + monst.width - 1, ru_coor_y = monst.y;                       // 右上
    int ld_coor_x = monst.x, ld_coor_y = monst.y + monst.height - 1;                      // 左下
    int rd_coor_x = monst.x + monst.width - 1, rd_coor_y = monst.y + monst.height - 1;    // 右下

    lu_coor_y += dy;
    ru_coor_y += dy;
    ld_coor_y += dy;
    rd_coor_y += dy;

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
bool bullet_is_collide_chara(int i, int dx, int dy) {
    int lu_coor_x = dx + bullets[i].x;                                                      // 左上
    int ru_coor_x = dx + bullets[i].x + bullets[i].width, ru_coor_y = dy + bullets[i].y;    // 右上
    int rd_coor_y = dy + bullets[i].y + bullets[i].height;                                  // 右下

    if (ru_coor_x > chara.x && chara.x + chara.width > lu_coor_x && rd_coor_y > chara.y && chara.y + chara.height > ru_coor_y) {
        return true;
    } else return false;
}
bool bullet_is_collide_monster(int i, int dx, int dy) {
    int lu_coor_x = dx + bullets[i].x;                                                      // 左上
    int ru_coor_x = dx + bullets[i].x + bullets[i].width, ru_coor_y = dy + bullets[i].y;    // 右上
    int rd_coor_y = dy + bullets[i].y + bullets[i].height;                                  // 右下

    if (ru_coor_x > monst.x && monst.x + monst.width > lu_coor_x && rd_coor_y > monst.y && monst.y + monst.height > ru_coor_y) {
        return true;
    } else return false;
}
int monst_chara_dist() {
    int monst_x = monst.x - monst.width / 2;
    int monst_y = monst.y - monst.height / 2;
    int chara_x = chara.x - chara.width / 2;
    int chara_y = chara.y - chara.height / 2;

    double dist = sqrt(pow(fabs(monst_x - chara_x), 2) + pow(fabs(monst_y - chara_y), 2));

    if ((int)(dist / 30) < 29) {
        return dist / 30;
    } else {
        return 29;
    }
}

void camera_update() {
    OBJECT_MODIFY_X = (WIDTH / 2) - (chara.x) - (chara.width / 2);
    OBJECT_MODIFY_Y = (HEIGHT / 2) - (chara.y) - (chara.height / 2);
}

typedef struct bar {
    int target;
    int x, y;
    int width, height;
    float length;
} Bar;

Bar bars[10];
int bar_max_id = 0;
void bar_init(int target, int x, int y, int width, int height) {
    bars[bar_max_id].target = target;    // target=0:monst target=1:chara
    bars[bar_max_id].length = 0;
    bars[bar_max_id].x = x;
    bars[bar_max_id].y = y;
    bars[bar_max_id].width = width;
    bars[bar_max_id].height = height;

    bar_max_id++;
}
void bar_update() {
    // bar
    for (int i = 0; i < bar_max_id; i++) {
        if (bars[i].target == 0) {    // monst
            bars[i].x = WIDTH / 6;
            bars[i].y = HEIGHT / 12;
            bars[i].width = (WIDTH / 3) * 2;
            bars[i].length = bars[i].width * ((float)monst.hp / (float)monst.MAX_HP);
        } else if (bars[i].target == 1) {    // chara
            bars[i].x = WIDTH / 2 - 30;
            bars[i].y = HEIGHT / 2 - 40;
            bars[i].length = bars[i].width * ((float)chara.hp / (float)chara.MAX_HP);
        }
    }
}
void bar_draw() {
    for (int i = 0; i < bar_max_id; i++) {
        // printf("length=%f x=%d y=%d width=%d height=%d\n", bars[i].length, bars[i].x, bars[i].y, bars[i].width, bars[i].height);
        al_draw_filled_rectangle(bars[i].x, bars[i].y, bars[i].x + bars[i].width, bars[i].y + bars[i].height, al_map_rgb(255, 255, 255));
        al_draw_filled_rectangle(bars[i].x, bars[i].y, bars[i].x + bars[i].length, bars[i].y + bars[i].height, al_map_rgb(255, 0, 0));
        if (bars[i].target == 0) al_draw_text(font, al_map_rgb(0, 0, 0), WIDTH / 2, bars[i].y + bars[i].height / 5, ALLEGRO_ALIGN_CENTER, "史萊姆王 Slime King");
    }

    int monst_x = monst.x - monst.width / 2;
    int monst_y = monst.y - monst.height / 2;
    int chara_x = chara.x - chara.width / 2;
    int chara_y = chara.y - chara.height / 2;
    // Draw the dist between chara and monst.
    // char temp[50];
    // sprintf(temp, "%llf", sqrt(pow(fabs(monst_x - chara_x), 2) + pow(fabs(monst_y - chara_y), 2)));
    // al_draw_text(font, al_map_rgb(0, 0, 0), 0, 0, 0, temp);
}

ALLEGRO_BITMAP* conti[2];
ALLEGRO_BITMAP* setting_[2];
ALLEGRO_BITMAP* exit_chara[2];
int now_on_ = -1;
enum { CONTI };
enum { MENU, FIGHT, SETTING, ABOUT, EXIT, EXIT_PAGE };
void pause_init() {
    conti[0] = al_load_bitmap("./image/conti_0.png");
    conti[1] = al_load_bitmap("./image/conti_1.png");
    setting_[0] = al_load_bitmap("./image/setting_0.png");
    setting_[1] = al_load_bitmap("./image/setting_1.png");
    exit_chara[0] = al_load_bitmap("./image/to_menu_0.png");
    exit_chara[1] = al_load_bitmap("./image/to_menu_1.png");
    setting_scene_init_chara();
}
void pause_update() {
    if (in_setting) {
        return;
    }

    if (key_state[ALLEGRO_KEY_ESCAPE]) {
        pause = false;
    }
}
void pause_process(ALLEGRO_EVENT event) {
    if (in_setting) {
        setting_scene_process_chara(event);
        return;
    }

    al_get_mouse_state(&mouse_state);
    if (event.type == ALLEGRO_EVENT_KEY_UP) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            pause = false;
        }
    }

    if (mouse_state.buttons & 1 && now_on_ == CONTI) {
        pause = false;
    } else if (mouse_state.buttons & 1 && now_on_ == SETTING) {
        in_setting = true;
    } else if (mouse_state.buttons & 1 && now_on_ == EXIT) {
        pause = false;
        aaa_scene = MENU;
        last_scene = FIGHT;
        judge_next_window = true;
    }
}
void pause_draw() {
    if (in_setting) {
        setting_scene_draw_chara();
        return;
    }

    al_draw_filled_rectangle(WIDTH / 4, HEIGHT / 3, WIDTH / 4 + WIDTH / 2, HEIGHT / 3 + HEIGHT / 3, al_map_rgb(100, 100, 100));

    int setting_x = WIDTH / 2 - al_get_bitmap_width(conti[0]) / 2, setting_y = HEIGHT / 2 - al_get_bitmap_height(conti[1]) / 2;
    int conti_x = setting_x - 220, conti_y = setting_y;
    int exit_x = setting_x + 220, exit_y = setting_y;

    al_draw_bitmap(conti[0], conti_x, conti_y, 0);
    al_draw_bitmap(setting_[0], setting_x, setting_y, 0);
    al_draw_bitmap(exit_chara[0], exit_x, exit_y, 0);

    if (mouse_state.x >= conti_x && mouse_state.x <= conti_x + al_get_bitmap_width(conti[0]) && mouse_state.y >= conti_y && mouse_state.y <= conti_y + al_get_bitmap_height(conti[0])) {
        now_on_ = CONTI;
        al_draw_bitmap(conti[1], conti_x, conti_y, 0);
    } else if (mouse_state.x >= setting_x && mouse_state.x <= setting_x + al_get_bitmap_width(setting_[0]) && mouse_state.y >= setting_y &&
               mouse_state.y <= setting_y + al_get_bitmap_height(setting_[0])) {
        now_on_ = SETTING;
        al_draw_bitmap(setting_[1], setting_x, setting_y, 0);
    } else if (mouse_state.x >= exit_x && mouse_state.x <= exit_x + al_get_bitmap_width(exit_chara[0]) && mouse_state.y >= exit_y && mouse_state.y <= exit_y + al_get_bitmap_height(exit_chara[0])) {
        now_on_ = EXIT;
        al_draw_bitmap(exit_chara[1], exit_x, exit_y, 0);
    } else {
        now_on_ = -1;
    }
}
void pause_destory() {
    al_destroy_bitmap(conti[0]);
    al_destroy_bitmap(conti[1]);
    al_destroy_bitmap(setting_[0]);
    al_destroy_bitmap(setting_[1]);
    al_destroy_bitmap(exit_chara[0]);
    al_destroy_bitmap(exit_chara[1]);
}

ALLEGRO_BITMAP* background_chara;
ALLEGRO_BITMAP* setting_boss_chara;
ALLEGRO_BITMAP* random_chara[2];
ALLEGRO_BITMAP* ql_chara[2];
int now_on_setting = -1;
void setting_scene_init_chara() {
    background_chara = al_load_bitmap("./image/background.jpg");
    setting_boss_chara = al_load_bitmap("./image/setting_boss.png");
    random_chara[0] = al_load_bitmap("./image/random_0.png");
    random_chara[1] = al_load_bitmap("./image/random_1.png");
    ql_chara[0] = al_load_bitmap("./image/ql_0.png");
    ql_chara[1] = al_load_bitmap("./image/ql_1.png");
}
void setting_scene_process_chara(ALLEGRO_EVENT event) {
    if (event.type == ALLEGRO_EVENT_KEY_UP) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            in_setting = false;
        }
    }

    al_get_mouse_state(&mouse_state);
    if (mouse_state.buttons & 1 && now_on_setting == RANDOM_MODE) {
        boss_mode = RANDOM_MODE;
    } else if (mouse_state.buttons & 1 && now_on_setting == QL_MODE) {
        boss_mode = QL_MODE;
    }
}
void setting_scene_draw_chara() {
    al_draw_bitmap(background_chara, 0, 0, 0);

    int setting_boss_x = 60, setting_boss_y = 60;
    int random_x = setting_boss_x + 500, random_y = setting_boss_y;
    int ql_x = random_x + 500, ql_y = setting_boss_y;

    al_draw_bitmap(setting_boss_chara, setting_boss_x, setting_boss_y, 0);

    if (boss_mode == RANDOM_MODE) {
        al_draw_bitmap(random_chara[1], random_x, random_y, 0);
        al_draw_bitmap(ql_chara[0], ql_x, ql_y, 0);
    } else if (boss_mode == QL_MODE) {
        al_draw_bitmap(random_chara[0], random_x, random_y, 0);
        al_draw_bitmap(ql_chara[1], ql_x, ql_y, 0);
    }

    if (mouse_state.x >= random_x && mouse_state.x <= random_x + al_get_bitmap_width(random_chara[0]) && mouse_state.y >= random_y &&
        mouse_state.y <= random_y + al_get_bitmap_height(random_chara[0])) {
        now_on_setting = RANDOM_MODE;
        al_draw_bitmap(random_chara[1], random_x, random_y, 0);
    } else if (mouse_state.x >= ql_x && mouse_state.x <= ql_x + al_get_bitmap_width(ql_chara[0]) && mouse_state.y >= ql_y && mouse_state.y <= ql_y + al_get_bitmap_height(ql_chara[0])) {
        now_on_setting = QL_MODE;
        al_draw_bitmap(ql_chara[1], ql_x, ql_y, 0);
    }
}
void setting_scene_destory_chara() {
    al_destroy_bitmap(setting_boss_chara);
    al_destroy_bitmap(random_chara[0]);
    al_destroy_bitmap(random_chara[1]);
    al_destroy_bitmap(ql_chara[0]);
    al_destroy_bitmap(ql_chara[1]);
}


void reset_positions(int chara_x, int chara_y, int monst_x, int monst_y) {
    chara.face_dir = UP;
    chara.x = chara_x;
    chara.y = chara_y;
    monst.x = monst_x;
    monst.y = monst_y;
}
bool check_end() {
    if (monst.hp <= 0 || chara.hp <= 0) {
        return true;
    } else return false;
}

void after_end() {
    // check hp state
    if (winner != 666 && winner != 444) {
        show_bar = false;

        if (monst.hp <= 0) winner = 666;
        else if (chara.hp <= 0) winner = 444;

        chara.state = BLOCK;
        monst.state = MONS_BLOCK;

        reset_positions(750, 900, 700, 700);

        // Sleep(3000);
        chara.state = STOP;
    }
}


ALLEGRO_BITMAP* chat_img;
void chat_init() {
    chat_img = al_load_bitmap("./image/chat.png");
}
void chat_process(ALLEGRO_EVENT event) {
    if (event.type == ALLEGRO_EVENT_KEY_UP) {
        if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
            chat_count++;
        }
    }
    // printf("[fin] load chat.%d\n", chat_count);
}
void chat_draw() {
    if (chat_count == 5 || chat_count == 11 || chat_count == 16) {
        in_chat = false;
        chat_count++;
        if (chat_count == 12 || chat_count == 17) {
            aaa_scene = EXIT_PAGE;
            last_scene = FIGHT;
            judge_next_window = true;
        }
        return;
    }
    int chat_x = WIDTH / 2 - al_get_bitmap_width(chat_img) / 2, chat_y = HEIGHT - al_get_bitmap_height(chat_img) / 2 - 160;    // y: 80
    al_draw_bitmap(chat_img, chat_x, chat_y, 0);
    al_draw_multiline_text(font, al_map_rgb(255, 255, 255), chat_x + 50, chat_y + 30, al_get_bitmap_width(chat_img) / 2, 0, 0, chat[chat_count]);
}
void chat_destory() {
    al_destroy_bitmap(chat_img);
}
void next_part_of_chat() {
    chat_count++;
    in_chat = true;
}

ALLEGRO_BITMAP* R_sign = NULL;
void sign_init() {
    R_sign = al_load_bitmap("./image/R_btn.png");
}
void sign_draw() {
    if (talkable == true) {
        int chief_village_R_x = 710 + OBJECT_MODIFY_X, chief_village_R_y = 2200 + OBJECT_MODIFY_Y - 50;
        int monst_R_x = monst.x + OBJECT_MODIFY_X + 70, monst_R_y = monst.y + OBJECT_MODIFY_Y - 70;
        al_draw_bitmap(R_sign, chief_village_R_x, chief_village_R_y, 0);
        al_draw_bitmap(R_sign, monst_R_x, monst_R_y, 0);
    }
}
void sign_update() {
    // check chat
    if (winner != -1 || chat_count == -1) {
        talkable = true;
        // printf("a\n");
    }
}
void sign_destroy() {
    al_destroy_bitmap(R_sign);
}

ALLEGRO_FONT* black_scene_font = NULL;
ALLEGRO_BITMAP* black_bkg;
void black_scene_init() {
    black_scene_font = al_load_ttf_font("./font/源樣明體-超粗.ttc", 32, 0);
    black_bkg = al_load_bitmap("./image/black.png");
}
void black_scene_draw() {
    al_draw_bitmap(black_bkg, 0, 0, 0);
    al_draw_text(black_scene_font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "經過了一番纏鬥，你和史萊姆王終於分出了勝負......");
}
void black_scene_destory() {
    al_destroy_bitmap(black_bkg);
}
