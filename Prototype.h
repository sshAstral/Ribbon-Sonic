// Prototype.h
#ifndef PROTOTYPE_H
#define PROTOTYPE_H

typedef enum { 
    STATE_TITLE, 
    STATE_LOADING_STAGE, 
    STATE_TEST_STAGE 
} GameState;

#ifdef __cplusplus
extern "C" {
#endif

int StartGameLoop(void* hInst, int nCmdShow);

#ifdef __cplusplus
}
#endif

#endif