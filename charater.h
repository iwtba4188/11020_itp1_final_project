#include "global.h"

void character_init();
void character_process(ALLEGRO_EVENT event);
void character_update();
void character_draw();
void character_destory();

void bullet_init(int team, int speed, float dx, float dy, int obtain_action, int init_monst_state_dist);
void bullets_update();
void bullets_draw();
void bullets_destory();

void monster_init();
void monster_process(ALLEGRO_EVENT event);
void monster_update();
void monster_draw();
void monster_destory();
void monst_go();

void chief_init();
void chief_of_village_draw();

void objects_init();
void objects_draw();

bool chara_is_collide_object();
bool chara_is_collide_portal();
bool monst_is_collide_object_x(int dx);
bool monst_is_collide_object_y(int dy);
bool bullet_is_collide_object(int i, int dx, int dy);
bool bullet_is_collide_chara(int i, int dx, int dy);
bool bullet_is_collide_monster(int i, int dx, int dy);
int monst_chara_dist();

void camera_update();

void bar_init(int target, int x, int y, int width, int height);
void bar_update();
void bar_draw();

void pause_init();
void pause_update();
void pause_process(ALLEGRO_EVENT event);
void pause_draw();
void pause_destory();

void setting_scene_init_chara();
void setting_scene_process_chara(ALLEGRO_EVENT event);
void setting_scene_draw_chara();
void setting_scene_destory_chara();

void reset_positions(int, int, int, int);
bool check_end();
void after_end();

void chat_init();
void chat_process(ALLEGRO_EVENT event);
void chat_draw();
void chat_destory();
void next_part_of_chat();

void sign_init();
void sign_draw();
void sign_update();
void sign_destroy();

void black_scene_init();
void black_scene_draw();
void black_scene_destory();