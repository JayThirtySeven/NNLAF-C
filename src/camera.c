#include <stdlib.h>

#include "camera.h"

camera_t *camera_create(void){
    camera_t *camera = malloc(sizeof(camera_t));
    camera->view = rect_create();
    camera->buffer = NULL;
    
    return camera;
}

void camera_init(camera_t *camera, int32_t w, int32_t h){
    camera->view->w = w;
    camera->view->h = h;
    camera->buffer = create_surface(w, h);
}

void camera_delete(camera_t *camera){
    rect_delete(camera->view);
    SDL_FreeSurface(camera->buffer);
}

void camera_fill_rect(camera_t *camera, rect_t *rect, int32_t color){
    SDL_Rect fill_rect;

    rect_copy_to_sdl(rect, &fill_rect);
    
    fill_rect.x -= camera->view->x;
    fill_rect.y -= camera->view->y;

    SDL_FillRect(camera->buffer, &fill_rect, color);
}

void camera_draw_sprite(camera_t *camera, sprite_t *sprite){
    SDL_Rect draw_rect;

    rect_copy_to_sdl(sprite->rect, &draw_rect);
    
    draw_rect.x -= camera->view->x;
    draw_rect.y -= camera->view->y;

    anim_draw(sprite->anim, sprite->step, camera->buffer, &draw_rect);
}

void camera_draw_game(camera_t *camera, game_t *game){
    camera->view->x = floor(camera->view->x);
    camera->view->y = floor(camera->view->y);

    SDL_FillRect(camera->buffer, NULL, 0x000000FF);

    rect_limit_to(camera->view, game->bounds);

    camera_fill_rect(camera, game->bounds, 0xDDDDDDFF);
    
    camera_draw_terrain_rects(camera, game);
    camera_draw_platform_rects(camera, game);
    camera_draw_targets(camera, game);
    camera_draw_enemies(camera, game);

    camera_draw_player(camera, game->player);
}

void camera_draw_terrain_rects(camera_t *camera, game_t *game){
    rect_node_t *iter = game->terrain_rects->head;
    while(iter != NULL){
        camera_fill_rect(camera, iter->data, 0x333366FF);
        iter = iter->next;
    }
}

void camera_draw_platform_rects(camera_t *camera, game_t *game){
    rect_node_t *iter = game->platform_rects->head;
    while(iter != NULL){
        camera_fill_rect(camera, iter->data, 0x7777AAFF);
        iter = iter->next;
    }
}

void camera_draw_player(camera_t *camera, player_t *player){
    
    if(player->flashing % 2 == 0){
        camera_fill_rect(camera, player->body->rect, 0x2222DDFF);
        camera_draw_sprite(camera, player->sprite);
    }
    
    camera_fill_rect(camera, player->weapon, 0xDD9900FF);
}

void camera_draw_enemies(camera_t *camera, game_t *game){
    enemy_node_t *iter = game->enemies->head;
    while(iter != NULL){
        if(iter->data->flashing % 2 == 0){
            camera_fill_rect(camera, iter->data->rect, 0xDD3333FF);
        }
        iter = iter->next;
    }
}

void camera_draw_targets(camera_t *camera, game_t *game){
    target_node_t *iter = game->targets->head;
    while(iter != NULL){
        camera_fill_rect(camera, iter->data->rect, iter->data->color);
        iter = iter->next;
    }
}

