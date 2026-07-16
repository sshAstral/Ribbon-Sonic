// Stage0.c
#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include "Stage0.h"
#include "MRS.h"

#pragma comment(lib, "msimg32.lib")

#define GENESIS_TOTAL_LIMIT 80
#define SCREEN_WIDTH  1680
#define SCREEN_HEIGHT 1050

GameObject stage_objects[GENESIS_TOTAL_LIMIT];
int total_objects = 0;

HBITMAP h_ground = NULL;
HBITMAP h_screen = NULL;

void init_stage0() {
    h_ground = (HBITMAP)LoadImage(NULL, "ASSETS\\RSSprites\\MAP\\PS-GROUND.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_screen = (HBITMAP)LoadImage(NULL, "ASSETS\\RSSprites\\MAP\\PS-SCREEN.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    if (!h_ground) printf("[G72 STAGE0] ERROR: No se encontro PS-GROUND.bmp\n");
    if (!h_screen) printf("[G72 STAGE0] ERROR: No se encontro PS-SCREEN.bmp\n");
    
    total_objects = 0; 
}

void render_stage0(HDC hdc_backbuffer, float player_x, float player_y) {
    HDC hdc_mem = CreateCompatibleDC(hdc_backbuffer);

    if (h_screen) {
        SelectObject(hdc_mem, h_screen);
        // Dibujamos el fondo estático en toda la pantalla (o puedes meterle scroll dividiendo camera_x)
        StretchBlt(hdc_backbuffer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                   hdc_mem,        0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SRCCOPY);
    }

    if (h_ground) {
        int camera_x = (int)player_x - (SCREEN_WIDTH / 2);
        if (camera_x < 0) camera_x = 0;

        SelectObject(hdc_mem, h_ground);
        // Dibujado de las baldosas de 32x32 en la coordenada Y 832
        for (int i = 0; i < 70; i++) {
            int tile_x = (i * 32) - camera_x;
            BitBlt(hdc_backbuffer, tile_x, 832, 32, 32, hdc_mem, 0, 0, SRCCOPY);
        }
    }

    DeleteDC(hdc_mem);
}

void render_sprites_with_genesis_limit(HDC hdc_back, HDC hdc_mem, Ribbon *player, HBITMAP h_player_bmp) {
    int sprites_drawn = 0;

    if (h_player_bmp != NULL) {
        SelectObject(hdc_mem, h_player_bmp);
        TransparentBlt(hdc_back, (int)player->x, (int)player->y, 32, 32, 
                       hdc_mem, 0, 0, 32, 32, RGB(0, 255, 0));
        sprites_drawn++;
    }

    for (int i = 0; i < total_objects; i++) {
        if (!stage_objects[i].active) continue;
        if (sprites_drawn >= GENESIS_TOTAL_LIMIT) break; 

        if (stage_objects[i].hBmp != NULL) {
            SelectObject(hdc_mem, stage_objects[i].hBmp);
            TransparentBlt(hdc_back, (int)stage_objects[i].x, (int)stage_objects[i].y, 32, 32, 
                           hdc_mem, 0, 0, 32, 32, RGB(0, 255, 0));
            sprites_drawn++;
        }
    }
}

void free_stage0() {
    if (h_ground) { DeleteObject(h_ground); h_ground = NULL; }
    if (h_screen) { DeleteObject(h_screen); h_screen = NULL; }
}
