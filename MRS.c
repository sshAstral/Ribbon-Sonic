#include "MRS.h"

#define BASE_SPEED        1.30f   
#define RUN_MULT          1.75f  
#define CHARGE_MULT       3.00f  
#define JUMP_FORCE       -12.0f  
#define GRAVITY           0.6f   
#define FRICTION          0.85f  

void update_ribbon_mechanics(Ribbon *p, float input_x, bool btn_charge, bool btn_jump_attack) {
    
    if (p->is_grounded && btn_charge) {
        p->vel_x = 0.0f;
        
        if (input_x != 0) {
            p->vel_x = input_x * (BASE_SPEED * CHARGE_MULT); 
        }
    } 
    else if (input_x != 0) {
        p->vel_x = input_x * (BASE_SPEED * RUN_MULT);
    } 
    else if (p->is_grounded) {
        p->vel_x *= FRICTION;
    }

    // SALTO
    if (p->is_grounded && btn_jump_attack) {
        p->vel_y = JUMP_FORCE;       
        p->is_grounded = false;     
        p->can_jump_attack = true;   
    }

    if (!p->is_grounded) {
        p->vel_y += GRAVITY;    
    }

    p->x += p->vel_x;
    p->y += p->vel_y;
}