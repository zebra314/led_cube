#ifndef __component__H__
#define __component__H__

#include "stm32l476xx.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_ADDRESS_DECODE_MODE		9
#define MAX_ADDRESS_ITENSITY		10
#define MAX_ADDRESS_SCAN_LIMIT		11
#define MAX_ADDRESS_SHUTDOWN		12
#define MAX_ADDRESS_DISPLAY_TEST	15

// Define pins for MAX7219
#define MAX7219_gpio GPIOB
#define MAX7219_DIN 3
#define MAX7219_CS 4
#define MAX7219_CLK 5

// Define pins for led (default use on-board led PA5)
#define LED_gpio GPIOA
#define LED_pin_built_in 5
#define LED_layer_3 10
#define LED_layer_4 9
#define LED_layer_2 6
#define LED_layer_1 7
#define LED_layer_0 8

#define Matrix_length 5
#define deafult_update_rate 100000
#define deafult_intensity 0xFF

typedef struct MAX7219{
    GPIO_TypeDef* gpio;
    int DIN;
    int CS;
    int CLK;
} MAX7219;

typedef struct LED_Matrix {
    GPIO_TypeDef* gpio;
    int layer_pins[Matrix_length];

    int update_rate ;
    int intensity ;

    // [x] : 0 ~ Matrix_length - 1
    // [y] : 0 ~ Matrix_length - 1
    // [z] : 0 ~ Matrix_length - 1
    int lighten_coordinate[Matrix_length][Matrix_length][Matrix_length]; // 0: off, 1: on
    
    // [z] : 0 ~ Matrix_length - 1
    int lighten_layer[Matrix_length]; // 0: off, 1: on
} LED_Matrix;


// Conponent initialization
LED_Matrix init_led_matrix(LED_Matrix *matrix, GPIO_TypeDef* gpio, int layer_pins[Matrix_length]);
MAX7219 init_max7219(MAX7219* max7219_interface, GPIO_TypeDef* gpio, int DIN, int CS, int CLK);
void init_led(GPIO_TypeDef* gpio, int LED_pin);
int init_button(GPIO_TypeDef* gpio, int button_pin);

// LED Matrix functions
void send_max7219(MAX7219* max7219_interface, int address, int data);
void choose_layer(LED_Matrix* matrix, int layer, int state);

// Basic GPIO functions
int read_gpio(GPIO_TypeDef* gpio, int pin);
void write_gpio(GPIO_TypeDef* gpio, int pin, int data);
void set_gpio(GPIO_TypeDef* gpio, int pin);
void reset_gpio(GPIO_TypeDef* gpio, int pin);
void toggle_output(GPIO_TypeDef* gpio, int pin);
void delay_without_interrupt(int msec);

int user_press_button(int debounce);
#endif