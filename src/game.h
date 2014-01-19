#ifndef game_h
#define game_h

typedef struct game_t game_t;

#include "core.h"

#include "font.h"
#include "menu.h"
#include "mixer.h"
#include "camera.h"
#include "player.h"
#include "controller.h"

#include "fset_dict.h"
#include "anim_dict.h"
#include "rect_list.h"
#include "enemy_list.h"
#include "target_list.h"


struct game_t{
    core_t *core;
    uint32_t step;
    uint32_t mode;

    font_t *font;

    controller_t *controller;
    
    menu_t *menu;
    
    mixer_t *mixer;
    camera_t *camera;   
    player_t *player;

    fset_dict_t *fsets;
    anim_dict_t *anims;
    
    rect_list_t *terrain_rects;
    rect_list_t *platform_rects;
    
    enemy_list_t *enemies;
    target_list_t *targets;
};

game_t *game_create(core_t *core);
void game_delete(game_t *game);

void game_fast_frame(game_t *game);
void game_full_frame(game_t *game);

#endif
