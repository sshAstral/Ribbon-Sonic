#ifndef MRS_H
#define MRS_H
#include <stdbool.h>

typedef struct {
    float x, y;
    float vel_x, vel_y;
    bool is_grounded;
    bool can_jump_attack;
} Ribbon;

void update_ribbon_mechanics(Ribbon *p, float input_x, bool btn_charge, bool btn_jump_attack);

#endif