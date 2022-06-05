#include "global.h"
// variables for global usage
ALLEGRO_FONT* font = NULL;
int now_scene = 1;
const float FPS = 60.0;
int WIDTH = 1600;
int HEIGHT = 900;
ALLEGRO_EVENT_QUEUE* event_queue = NULL;
ALLEGRO_TIMER* fps = NULL;
bool key_state[ALLEGRO_KEY_MAX] = {false};
bool judge_next_window = false;
const int BLOCK_WIDTH = 50;
const int BLOCK_HEIGHT = 50;
int THIS_SCENE_SIZE_ROW = 0;
int THIS_SCENE_SIZE_COL = 0;
int THIS_SCENE_MAP[MAX_SCENE_SIZE_ROW][MAX_SCENE_SIZE_COL];
int OBJECT_MODIFY_X = 0;
int OBJECT_MODIFY_Y = 0;