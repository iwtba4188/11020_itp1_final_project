#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED
#define GAME_TERMINATE -1
#define MAX_SCENE_SIZE_ROW 100
#define MAX_SCENE_SIZE_COL 100

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// note that you can't assign initial value here!
extern int boss_mode;

extern bool in_chat;
extern int chat_count;
extern char chat[500][500];
extern bool in_setting;
extern bool pause;
extern bool terminate;
extern int last_scene;
extern int aaa_scene;
extern bool show_bar;
extern bool black_scene;
extern ALLEGRO_FONT* font;
extern int now_scene;
extern const float FPS;
extern int WIDTH;
extern int HEIGHT;
extern bool key_state[ALLEGRO_KEY_MAX];
extern bool judge_next_window;
extern ALLEGRO_EVENT_QUEUE* event_queue;
extern ALLEGRO_TIMER* fps;
extern const int BLOCK_WIDTH;
extern const int BLOCK_HEIGHT;
extern int THIS_SCENE_SIZE_ROW;
extern int THIS_SCENE_SIZE_COL;
extern int THIS_SCENE_MAP[MAX_SCENE_SIZE_ROW][MAX_SCENE_SIZE_COL];
extern int OBJECT_MODIFY_X;
extern int OBJECT_MODIFY_Y;
extern int winner;
#endif
