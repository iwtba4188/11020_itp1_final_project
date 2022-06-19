#include "GameWindow.h"
#include <windows.h>

bool draw = false;
enum { MENU, FIGHT, SETTING, ABOUT, EXIT, EXIT_PAGE };

const char* title = "守護家園";

// ALLEGRO Variables
ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_SAMPLE* song = NULL;
ALLEGRO_SAMPLE_INSTANCE* sample_instance;

int Game_establish() {
    int msg = 0;

    game_init();
    game_begin();

    while (msg != GAME_TERMINATE) {
        msg = game_run();
        if (msg == GAME_TERMINATE) printf("Game Over\n");
    }

    game_destroy();
    return 0;
}

void game_init() {
    printf("Game Initializing...\n");
    al_init();
    // init audio
    al_install_audio();
    al_init_acodec_addon();
    // Create display
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    display = al_create_display(WIDTH, HEIGHT);
    // create event queue
    event_queue = al_create_event_queue();
    // Initialize Allegro settings
    al_set_window_position(display, 0, 0);
    al_set_window_title(display, title);
    al_init_primitives_addon();
    al_init_font_addon();      // initialize the font addon
    al_init_ttf_addon();       // initialize the ttf (True Type Font) addon
    al_init_image_addon();     // initialize the image addon
    al_init_acodec_addon();    // initialize acodec addon
    al_install_keyboard();     // install keyboard event
    al_install_mouse();        // install mouse event
    al_install_audio();        // install audio event
    // Register event
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    fps = al_create_timer(1.0 / FPS);
    al_register_event_source(event_queue, al_get_timer_event_source(fps));
    // initialize the icon on the display
    ALLEGRO_BITMAP* icon = al_load_bitmap("./image/icon.png");
    al_set_display_icon(display, icon);
}

void game_begin() {
    // Load sound
    song = al_load_sample("./sound/hello.mp3");
    al_reserve_samples(20);
    sample_instance = al_create_sample_instance(song);
    // Loop the song until the display closes
    al_set_sample_instance_playmode(sample_instance, ALLEGRO_PLAYMODE_LOOP);
    al_restore_default_mixer();
    al_attach_sample_instance_to_mixer(sample_instance, al_get_default_mixer());
    // set the volume of instance
    al_set_sample_instance_gain(sample_instance, 1);
    al_play_sample_instance(sample_instance);
    al_start_timer(fps);
    // initialize the menu before entering the loop
    menu_init();
    now_scene = MENU;
}
void game_update() {
    if (judge_next_window) {
        if (aaa_scene == MENU) {
            // not back menu anymore, therefore destroy it
            menu_init();
        } else if (aaa_scene == FIGHT) {
            game_scene1_init();
        } else if (aaa_scene == SETTING) {
            setting_scene_init();
        } else if (aaa_scene == ABOUT) {
            about_scene_init();
        } else if (aaa_scene == EXIT_PAGE) {
            exit_fighting_scene_init();
        }
        now_scene = aaa_scene;
        al_set_target_backbuffer(display);
        judge_next_window = false;
    }
    if (now_scene == FIGHT) {
        // XXX: object updates
        if (!pause) {
            character_update();
            camera_update();
            bullets_update();
            monster_update();
            bar_update();
        } else {
            pause_update();
        }
    }
}
void window_size_process(ALLEGRO_EVENT event) {
    if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
        al_acknowledge_resize(display);
        WIDTH = al_get_display_width(display);
        HEIGHT = al_get_display_height(display);
    }
}
int process_event() {
    // Request the event
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);
    // process the event of other component
    window_size_process(event);
    if (now_scene == MENU) {
        menu_process(event);
    } else if (now_scene == FIGHT) {
        // XXX: object process
        if (!pause) {
            if (!in_chat) {
                character_process(event);
                monster_process(event);
            } else {
                chat_process(event);
            }
        } else {
            pause_process(event);
        }
    } else if (now_scene == SETTING) {
        setting_scene_process(event);
    } else if (now_scene == ABOUT) {
        about_scene_process(event);
    } else if (now_scene == EXIT_PAGE) {
        exit_fighting_scene_process(event);
    }

    // Shutdown our program
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || terminate == true) return GAME_TERMINATE;
    else if (event.type == ALLEGRO_EVENT_TIMER)
        if (event.timer.source == fps) draw = true;
    if (draw) game_update();
    return 0;
}
void game_draw() {
    if (now_scene == MENU) {
        menu_draw();
    } else if (now_scene == FIGHT) {
        if (!pause) game_scene1_draw();
        else pause_draw();
    } else if (now_scene == SETTING) {
        setting_scene_draw();
    } else if (now_scene == ABOUT) {
        about_scene_draw();
    } else if (now_scene == EXIT_PAGE) {
        exit_fighting_scene_draw();
    }
    al_flip_display();
}
int game_run() {
    int error = 0;
    if (draw) {
        game_draw();
        draw = false;
    }
    if (!al_is_event_queue_empty(event_queue)) {
        error = process_event();
    }
    return error;
}

void game_destroy() {
    // Make sure you destroy all things
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    game_scene1_destroy();
    setting_scene_destory();
    about_scene_destroy();
    menu_destroy();
}
