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
ALLEGRO_FONT* font = NULL;
int now_scene = 0;
int winner = -1;

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

char chat[500][500] = {"[村長]　終於到了這一天，是時候打敗這隻神秘的(?)史\n萊姆來守護家園吧！",
                       "[你]　......",
                       "[你]　蛤？我只是路過而已欸...",
                       "[村長]　我知道～但我們的相遇是個美妙的緣分，相信你\n一定會成功的！",
                       "[你]　欸不是，可是我真的沒有很想守護家園欸...",
                       "===next part(player win)===",
                       "[史萊姆王]　沒想到你竟然可以打敗我，果然後生可畏阿！",
                       "[史萊姆王]　看來守護家園的重責大任應該交給你了，變\n成史萊姆之後，要記得回來看看我們啊～",
                       "[你]　Wait.a.minute!　什麼叫做變成「史萊姆」？！",
                       "[史萊姆王]　就是字面上的意思❤(ӦｖӦ｡)不然你以為那麼\n多年的和平是怎麼來的。",
                       "[史萊姆王]　好啦，我該離開了。接下來是年輕人的時代阿～",
                       "===next part(slime win)===",
                       "[史萊姆王]　看來你的實力還不夠守護你的家園阿～再回家\n多練練吧，年輕人。",
                       "[你]　......",
                       "[你]　我真的沒有很想守護ㄐ...",
                       "[史萊姆王]　不！我相信你一定可以做到的。後會有期，年\n輕人～"};
int chat_count = -1;