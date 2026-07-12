// Stage0.h
#ifndef STAGE0_H
#define STAGE0_H

#include <windows.h>
#include "MRS.h"

typedef struct {
    float x, y;
    HBITMAP hBmp;
    bool active;
} GameObject;

void init_stage0();
void render_stage0(HDC hdc_backbuffer, float player_x, float player_y);
void render_sprites_with_genesis_limit(HDC hdc_back, HDC hdc_mem, Ribbon *player, HBITMAP h_player_bmp);
void free_stage0();

#endif