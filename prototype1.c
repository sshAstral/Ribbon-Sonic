// Prototype1.c
#include "Prototype.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdbool.h>
#include "MRS.h"
#include "Stage0.h"
#pragma comment(lib, "msimg32.lib")

#define TOTAL_TITLE_FRAMES 11
#define SCREEN_WIDTH  1680
#define SCREEN_HEIGHT 1050

GameState current_state = STATE_TITLE;
Ribbon player = {100.0f, 800.0f, 0.0f, 0.0f, true, true};

bool keys[256]; 

HBITMAP h_tex_normal = NULL;
HBITMAP h_tex_running = NULL;
HBITMAP h_tex_titles[TOTAL_TITLE_FRAMES + 1];
HBITMAP h_tex_loading_screen = NULL; 

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_KEYDOWN:
            if (wParam < 256) keys[wParam] = true;
            break;
        case WM_KEYUP:
            if (wParam < 256) keys[wParam] = false;
            break;
        case WM_DESTROY:
            ChangeDisplaySettings(NULL, 0);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int StartGameLoop(void* hInst, int nCmdShow) {
    HINSTANCE hInstance = (HINSTANCE)hInst;

    memset(keys, 0, sizeof(keys));

    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    printf("[G72 LOG] Instancia del bucle iniciada correctamente.\n");

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Ribbon";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    printf("[G72 LOG] Cambiando configuracion de pantalla a %dx%d...\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    DEVMODE dmScreenSettings;
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = SCREEN_WIDTH;
    dmScreenSettings.dmPelsHeight = SCREEN_HEIGHT;
    dmScreenSettings.dmBitsPerPel = 32; 
    dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
        printf("[G72 WARNING] El modo pantalla completa fallo. Usando ventana por defecto.\n");
    }

    HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW, 
                               "Ribbon", 
                               "Ribbon -NT 4.0 Beta 0", 
                               WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
                               0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                               NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        printf("[G72 CRITICAL] Error de CreateWindowEx.\n");
        ChangeDisplaySettings(NULL, 0);
        return 0;
    }

    printf("[G72 LOG] Mapeando assets desde almacenamiento...\n");
    h_tex_normal  = (HBITMAP)LoadImage(NULL, "ASSETS\\RSSprites\\SONIC\\PS-NORMAL.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_tex_running = (HBITMAP)LoadImage(NULL, "ASSETS\\RSSprites\\SONIC\\PS-RUNNING.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    h_tex_loading_screen = (HBITMAP)LoadImage(NULL, "ASSETS\\RSSprites\\TITLEs\\Stage0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    for (int i = 1; i <= TOTAL_TITLE_FRAMES; i++) {
        char path_title[260];
        snprintf(path_title, sizeof(path_title), "ASSETS\\RSSprites\\TITLEs\\title%d.bmp", i);
        h_tex_titles[i] = (HBITMAP)LoadImage(NULL, path_title, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }

    int title_frame = 1;
    int anim_counter = 0;
    MSG msg;
    bool running = true;

    ShowWindow(hwnd, SW_SHOWMAXIMIZED); 
    UpdateWindow(hwnd); 
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    HDC hdc_win = GetDC(hwnd);
    HDC hdc_back = CreateCompatibleDC(hdc_win);
    HBITMAP h_back_bmp = CreateCompatibleBitmap(hdc_win, SCREEN_WIDTH, SCREEN_HEIGHT);
    SelectObject(hdc_back, h_back_bmp);

    PlaySound("ASSETS\\Music\\SegaCD_BIOS.wav", NULL, SND_FILENAME | SND_ASYNC);

    MSG flush_msg;
    while (PeekMessage(&flush_msg, NULL, 0, 0, PM_REMOVE)) {}
    memset(keys, 0, sizeof(keys));

    printf("[G72 LOG] Entrando al bucle principal de juego...\n");

    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) running = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (keys[VK_ESCAPE]) {
            running = false;
        }

        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
        RECT rect_screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        FillRect(hdc_back, &rect_screen, hBrush);
        DeleteObject(hBrush);

        HDC hdc_mem = CreateCompatibleDC(hdc_back);
        bool frame_renderizado = false;

        switch (current_state) {
            case STATE_TITLE:
                if (h_tex_titles[title_frame]) {
                    SelectObject(hdc_mem, h_tex_titles[title_frame]);
                    StretchBlt(hdc_back, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                               hdc_mem,  0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SRCCOPY);
                    frame_renderizado = true;
                }

                anim_counter++;
                if (anim_counter >= 12) {
                    if (title_frame < 8) {
                        title_frame++;
                    } 
                    else {
                        title_frame++;
                        if (title_frame > 11) {
                            title_frame = 8;
                        }
                    }
                    anim_counter = 0;
                }

                if (keys[VK_RETURN]) { 
                    printf("[G72 LOG] Avanzando desde el menu de titulo. Deteniendo audio del BIOS...\n");
                    PlaySound(NULL, NULL, 0); 
                    current_state = STATE_LOADING_STAGE;
                }
                break;

            case STATE_LOADING_STAGE:
                if (h_tex_loading_screen != NULL) {
                    SelectObject(hdc_mem, h_tex_loading_screen);
                    StretchBlt(hdc_back, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                               hdc_mem,  0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SRCCOPY);
                    frame_renderizado = true;
                }

                BitBlt(hdc_win, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hdc_back, 0, 0, SRCCOPY);
                DeleteDC(hdc_mem);

                printf("[G72 LOG] Pantalla Stage0.bmp volcada. Cargando datos...\n");
                init_stage0();
                Sleep(1500); 

                current_state = STATE_TEST_STAGE;
                printf("[G72 LOG] Carga completa. Pasando a STATE_TEST_STAGE.\n");

                hdc_mem = CreateCompatibleDC(hdc_back); 
                break;

            case STATE_TEST_STAGE:
                {
                    float input_x = 0.0f;
                    if (keys[VK_RIGHT]) input_x = 1.0f;
                    if (keys[VK_LEFT])  input_x = -1.0f;
                    
                    bool btn_charge = keys['S'];        
                    bool btn_jump = keys[VK_SPACE];     
                    if (player.y >= 800.0f) {
                        player.y = 800.0f;        
                        player.vel_y = 0.0f;      
                        player.is_grounded = true; 
                    } else {
                        player.is_grounded = false;
                    }

                    update_ribbon_mechanics(&player, input_x, btn_charge, btn_jump);
                    
                    render_stage0(hdc_back, player.x, player.y);

                    HBITMAP h_active_player = h_tex_normal;
                    if (player.vel_x > 1.2f || player.vel_x < -1.2f) {
                        h_active_player = h_tex_running;
                    }

                    if (h_active_player) {
                        SelectObject(hdc_mem, h_active_player);
                        TransparentBlt(hdc_back, (int)player.x, (int)player.y, 32, 32, 
                                       hdc_mem, 0, 0, 32, 32, RGB(0, 255, 0));
                    }
                    frame_renderizado = true;
                }
                break;
        }

        DeleteDC(hdc_mem);

        if (current_state != STATE_LOADING_STAGE && frame_renderizado) {
            BitBlt(hdc_win, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hdc_back, 0, 0, SRCCOPY);
        }
        
        Sleep(16); 
    }

    PlaySound(NULL, NULL, 0);
    free_stage0();
    DeleteObject(h_tex_normal);
    DeleteObject(h_tex_running);
    DeleteObject(h_tex_loading_screen);
    for (int i = 1; i <= TOTAL_TITLE_FRAMES; i++) {
        if (h_tex_titles[i]) DeleteObject(h_tex_titles[i]);
    }
    DeleteObject(h_back_bmp);
    DeleteDC(hdc_back);
    ReleaseDC(hwnd, hdc_win);

    ChangeDisplaySettings(NULL, 0);

    return 0;
}