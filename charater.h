#include "global.h"

void character_init();
void character_process(ALLEGRO_EVENT event);
void character_update();
void character_draw();
void character_destory();

void bullet_init(int speed);
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
bool bullet_is_collide_object(int i, int dx, int dy);

void camera_update();