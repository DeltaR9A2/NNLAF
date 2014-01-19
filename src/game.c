#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <tgmath.h>

#include <SDL2/SDL.h>

#include "game.h"
#include "loader.h"
#include "rect.h"

static const uint32_t MODE_MENU = 0;
static const uint32_t MODE_PLAY = 1;

void game_create_data_structures(game_t *game);
void game_delete_data_structures(game_t *game);

#ifdef DEBUG
#define DEBUG_MSG_LEN 128
uint32_t debug_message_timeout = 0;
wchar_t debug_message[DEBUG_MSG_LEN];
SDL_Surface *debug_message_surface;

void set_debug_message(const wchar_t *text){
	debug_message_timeout = 240;
	swprintf(debug_message, DEBUG_MSG_LEN, text);
}
#else
void set_debug_message(const wchar_t *text){
	return;
}
#endif

void game_exit(game_t *game){
	game->core->running = false;
}

void game_new_game(game_t *game){
	game->mode = MODE_PLAY;
}

void game_load_game(game_t *game){
	set_debug_message(L"'Load Game' not implemented yet.");
}

void game_options_menu(game_t *game){
	set_debug_message(L"'Options' not implemented yet.");
}

game_t *game_create(core_t *core){
    game_t *game = malloc(sizeof(game_t));
    
    game->core = core;
    game->step = 0;
    game->mode = MODE_MENU;
    
    game_create_data_structures(game);

	menu_add_option(game->menu, L"New Game", &game_new_game);
	menu_add_option(game->menu, L"Load Game", &game_load_game);
	menu_add_option(game->menu, L"Options", &game_options_menu);
	menu_add_option(game->menu, L"Exit", &game_exit);

    game->font = font_create("font_8bit.png");

    camera_init(game->camera, 640, 360);
    rect_init(game->camera->bounds, 0, 0, 1024, 1024);
    
    load_game(game);
    
    #ifdef DEBUG
    debug_message_surface = create_surface(640, 3+font_get_height(game->font));
    #endif

    return game;
}

void game_delete(game_t *game){
    font_delete(game->font);
    
    game_delete_data_structures(game);
    
    free(game);
}

void game_update_enemies(game_t *game){
    enemy_node_t *iter;
    for(iter = game->enemies->head; iter; iter = iter->next){
        enemy_update(iter->data, game);
    }
}

void game_check_enemies(game_t *game){
    enemy_node_t *iter;
    for(iter = game->enemies->head; iter; iter = iter->next){
        if(iter->data->flashing > 0){
            iter->data->flashing -= 1;
        }else{
            if(rect_overlap(iter->data->rect, game->player->weapon)){
                iter->data->flashing = 15;
                printf("Enemy hit by player.\n");
            }
        }

        if(game->player->flashing == 0){
            if(rect_overlap(iter->data->weapon, game->player->body->rect)){
                game->player->flashing = 60;
                printf("Player hit by enemy.\n");
            }
        }
    }
}

void game_check_targets(game_t *game){
    for(target_node_t *iter = game->targets->head; iter; iter = iter->next){
        if(rect_overlap(iter->data->rect, game->player->body->rect)){
            if(controller_just_pressed(game->controller, BTN_X)){
                (*iter->data->action)(iter->data, game);
            }
        }
    }
}

void game_fast_frame(game_t *game){
    game->step += 1;

	if(game->mode == MODE_MENU){
		if(controller_just_pressed(game->controller, BTN_U)){
			menu_up(game->menu);
		}
		if(controller_just_pressed(game->controller, BTN_D)){
			menu_down(game->menu);
		}
		if(controller_just_pressed(game->controller, BTN_A)){
			menu_activate(game->menu);
		}
	}
	
	if(game->mode == MODE_PLAY){
		player_update(game->player, game);
		game_update_enemies(game);
		game_check_enemies(game);
		game_check_targets(game);
	}
}

void game_full_frame(game_t *game){
    game_fast_frame(game);
    
    if(game->mode == MODE_MENU){
		SDL_FillRect(game->core->screen, NULL, 0x000000FF);
		menu_draw(game->menu, game->core->screen);
    }
    
    if(game->mode == MODE_PLAY){
		rect_move_to(game->camera->view, game->player->body->rect);
		camera_draw_game(game->camera, game);
		SDL_BlitSurface(game->camera->buffer, NULL, game->core->screen, NULL);
	}

	#ifdef DEBUG
	if(debug_message_timeout > 0){
		debug_message_timeout -= 1;
		SDL_FillRect(debug_message_surface, NULL, 0x000000AA);
		font_draw_string(game->font, debug_message, 4, 2, debug_message_surface);
		SDL_BlitSurface(debug_message_surface, NULL, game->core->screen, NULL);
	}
	#endif
	
}

void game_create_data_structures(game_t *game){
	game->menu = menu_create(game);
	
    game->controller = controller_create();
    game->mixer = mixer_create();
    game->camera = camera_create();
    game->player = player_create();
    
    game->fsets = fset_dict_create();
    game->anims = anim_dict_create();

    game->terrain_rects = rect_list_create();
    game->platform_rects = rect_list_create();

    game->enemies = enemy_list_create();
    game->targets = target_list_create();
}

void game_delete_data_structures(game_t *game){
    target_list_delete(game->targets);
    enemy_list_delete(game->enemies);

    rect_list_delete(game->platform_rects);
    rect_list_delete(game->terrain_rects);
    anim_dict_delete(game->anims);
    fset_dict_delete(game->fsets);
    
    player_delete(game->player);
    camera_delete(game->camera);
    mixer_delete(game->mixer);
    controller_delete(game->controller);
    
    menu_delete(game->menu);
}

