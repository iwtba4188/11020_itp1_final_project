#include "global.h"
// variables for global usage
int boss_mode = 0;

bool in_chat = false;
bool in_setting = false;
bool pause = false;
bool terminate = false;
int last_scene = -1;
int aaa_scene = -1;
bool show_bar = false;
bool black_scene = false;
ALLEGRO_FONT* font = NULL;
int now_scene = 0;
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
int winner = -1;

char chat[500][500] = {"[村長]　終於到了這一天，是時候打敗這隻\n神秘的(?)史萊姆來守護家園吧！",
                       "[你]　...",
                       "[你]　蛤？",
                       "[你]　欸不是，可是我沒有很想守護家園欸...",
                       "===next part===",
                       "[史萊姆王]　沒想到你竟然可以打敗我，果然後生可畏阿！",
                       "[史萊姆王]　看來守護家園的重責大任應該交給你了，變成史萊姆之後，要記得回來看看我們啊～",
                       "[你]　Wait a minute. 什麼叫做變成「史萊姆」？！"};
int chat_count = -1;