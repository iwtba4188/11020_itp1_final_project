#include "global.h"

void character_init();
void character_process(ALLEGRO_EVENT event);
void character_update();
void character_draw();
void character_destory();

void bullet_init(int team, int speed, float dx, float dy);
void bullets_update();
void bullets_draw();
void bullets_destory();

void monster_init();
void monster_process(ALLEGRO_EVENT event);
void monster_update();
void monster_draw();
void monster_destory();

void objects_draw();

bool chara_is_collide_object();
bool monst_is_collide_object_x(int dx);
bool monst_is_collide_object_y(int dy);
bool bullet_is_collide_object(int i, int dx, int dy);
bool bullet_is_collide_chara(int i, int dx, int dy);
bool bullet_is_collide_monster(int i, int dx, int dy);

void camera_update();

void bar_init(int target, int x, int y, int width, int height);
void bar_update();
void bar_draw();