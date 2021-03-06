#include <stdlib.h>

#include "player.h"

const uint32_t DIR_X = 0;
const uint32_t DIR_R = 1;
const uint32_t DIR_L = 2;

player_t *player_create(void){
	player_t *player = malloc(sizeof(player_t));
	
	player->body = body_create();
	player->sprite = sprite_create();
	
	player->flashing = 0;
   
	player->body->rect->x = 0;
	player->body->rect->y = 0;
	player->body->rect->w = 24;
	player->body->rect->h = 38;
	
	player->fall_speed = 10.0;
	player->fall_accel = 0.35;
	
	player->ground_speed = 4.00;
	player->ground_accel = 0.25;
	player->ground_decel = 0.25;
	
	player->jump_force = -8.2;
	player->jump_brake = -3.0;

	player->face_dir = DIR_R;
	player->ctrl_dir = DIR_X;
	player->move_dir = DIR_X;

	return player;
}

void player_delete(player_t *player){
	sprite_delete(player->sprite);
	body_delete(player->body);
	free(player);
}

void player_update(player_t *player, game_t *game){
	if(player->flashing > 0){
		player->flashing -= 1;
	}
	
	player_update_controls(player, game);
	player_update_animation(player, game);
	do_physics_to_it(player->body, game->active_map->terrain_rects, game->active_map->platform_rects);
	rect_move_to(player->sprite->rect, player->body->rect);
}

void player_update_controls(player_t *player, game_t *game){
	player->body->flags &= ~PLAT_DROP;

	if(player->body->vy < player->fall_speed){
		player->body->vy = fmin(player->fall_speed, player->body->vy + player->fall_accel);
	}


	if(controller_pressed(game->controller, BTN_D)){
		player->body->flags |= PLAT_DROP;
	}
	
	if(controller_just_pressed(game->controller, BTN_U)){
		if(player->body->flags & BLOCKED_D && controller_released(game->controller, BTN_D)){
			player->body->vy = player->jump_force;
			mixer_test_sound(game->mixer);
		}
	}else if(controller_just_released(game->controller, BTN_U)){
		if(player->body->vy < player->jump_brake){
			player->body->vy = player->jump_brake;
		}
	}
	
	if(controller_pressed(game->controller, BTN_R)){
		player->face_dir = DIR_R;
		player->ctrl_dir = DIR_R;
		if(player->body->vx < 0){
			player->body->vx += player->ground_accel;
		}else if(player->body->vx < player->ground_speed){
			player->body->vx = fmin(player->ground_speed, player->body->vx + player->ground_accel);
		}
	}else if(controller_pressed(game->controller, BTN_L)){
		player->face_dir = DIR_L;
		player->ctrl_dir = DIR_L;
		if(player->body->vx > 0){
			player->body->vx -= player->ground_accel;
		}else if(player->body->vx > -player->ground_speed){
			player->body->vx = fmax(-player->ground_speed, player->body->vx - player->ground_accel);
		}
	}else{
		player->ctrl_dir = DIR_X;
		if(player->body->vx > 0.0){
			player->body->vx = fmax(0.0, player->body->vx - player->ground_decel);
		}else if(player->body->vx < 0.0){
			player->body->vx = fmin(0.0, player->body->vx + player->ground_decel);
		}
	}
	
	if(player->body->vx > 0 && !(player->body->flags & BLOCKED_R)){
		player->move_dir = DIR_R;
	}else if(player->body->vx < 0 && !(player->body->flags & BLOCKED_L)){
		player->move_dir = DIR_L;
	}else{
		player->move_dir = DIR_X;
	}
}

void player_update_animation(player_t *player, game_t *game){
	player->sprite->step += 1;

	if(player->body->flags & BLOCKED_D){
		if(player->move_dir == DIR_X){
			if(player->face_dir == DIR_R){
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_idle_r"));
			}else if(player->face_dir == DIR_L){
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_idle_l"));
			}
		}else if(player->move_dir == DIR_R){
			if(player->ctrl_dir == DIR_R){
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_move_r"));
			}else{
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_skid_r"));
			}
		}else if(player->move_dir == DIR_L){
			if(player->ctrl_dir == DIR_L){
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_move_l"));
			}else{
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_skid_l"));
			}
		}
	}else{
		if(player->face_dir == DIR_R){
			if(player->body->vy < -1.0){
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_jump_r"));
			}else if(player->body->vy > 1.0){
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_fall_r"));
			}else{
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_hang_r"));
			}
		}else if(player->face_dir == DIR_L){
			if(player->body->vy < -1.0){
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_jump_l"));
			}else if(player->body->vy > 1.0){
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_fall_l"));
			}else{
				sprite_set_anim(player->sprite, anim_dict_get(game->anims, "generic_hang_l"));
			}
		}
	}
}

