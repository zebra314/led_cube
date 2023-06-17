#ifndef __ANIMATION__H__
#define __ANIMATION__H__

#include "component.h"

typedef struct Point{
    float x_pos;
    float y_pos;
    float z_pos;

    float x_dir;
    float y_dir;
    float z_dir;
} Point;

void display(MAX7219* max7219_interface, LED_Matrix* led_matrix);
Point colliding(Point ball);
#endif
