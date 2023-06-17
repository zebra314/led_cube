#include "animation.h"
#include "component.h"

void display(MAX7219* max7219_interface, LED_Matrix* led_matrix){
    for(int layer = 0; layer < Matrix_length; layer++){
        if(led_matrix->lighten_layer[layer])
            choose_layer(led_matrix, layer, 1);
        for(int y = 0; y < Matrix_length; y++){
            int output = 0, tmp = 0;
            for(int x = 0; x < Matrix_length; x++){
                if(led_matrix->lighten_coordinate[x][y][layer] == 1) {
                    tmp = (0b10000000>>(x));
                    output = output | tmp;
                }
            }
            send_max7219(max7219_interface, y+1, output);
        }
    }
}

Point colliding(Point ball){
    Point new_point;
    float x_dir = ball.x_dir;
    float y_dir = ball.y_dir;
    float z_dir = ball.z_dir; 

    float x = ball.x_pos;
    float y = ball.y_pos;
    float z = ball.z_pos;

    // if collide
    // int isCollide = 0;
    if(x<1 || x>5){
        x_dir*=-1;
        // isCollide = 1;
    }
    if(y<1 || y>5){
        y_dir*=-1;
        // isCollide = 1;
    }
    if(z<1 || z>5){
        z_dir*=-1;
        // isCollide = 1;
    }
    
    // if(isCollide == 0){
        x+=x_dir;
        y+=y_dir;
        z+=z_dir;
    // }
    new_point.x_dir = x_dir;
    new_point.y_dir = y_dir;
    new_point.z_dir = z_dir;
    new_point.x_pos = x;
    new_point.y_pos = y;
    new_point.z_pos = z;
    
    return new_point; 
}