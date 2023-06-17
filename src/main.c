#include "stm32l476xx.h"
#include "component.h"
#include "animation.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

// Define Counter timer
// #define COUNTER_timer TIM2

LED_Matrix test_matrix;
MAX7219 max7219_interface;

Point point = {	.x_pos = 5,
				.y_pos = 5,
				.z_pos = 5,
				.x_dir = -0.348,
				.y_dir = -0.384,
				.z_dir = -0.87,
			};

// Check if the program is running
#define DEBUG_led 
void SysTick_Handler() {
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		// Toggle LED display
		#ifdef DEBUG_led
		toggle_output(LED_gpio, LED_pin_built_in);
		#endif
		Point new_point = colliding(point);
		test_matrix.lighten_coordinate[(int)(point.x_pos)][(int)(point.y_pos)][(int)(point.z_pos)] = 1;
		display(&max7219_interface, &test_matrix);
		point = new_point;
	}
}

int main()
{
	// init_led(GPIOA, LED_pin_built_in);

	// Initialize LED Matrix
	int layer_pins[Matrix_length] = {LED_layer_0, LED_layer_1, LED_layer_2, LED_layer_3, LED_layer_4};
	test_matrix = init_led_matrix(&test_matrix, LED_gpio, layer_pins);

	// Initialize MAX7219
	max7219_interface = init_max7219(&max7219_interface, MAX7219_gpio, MAX7219_DIN, MAX7219_CS, MAX7219_CLK);

	// Configure SysTick clk to 10 Mhz and interrupt every 1 sec 
	// SystemClock_Config_Interrupt(10, 100000);

    while(1) {
		set_gpio(GPIOA, layer_pins[0]);
		send_max7219(&max7219_interface, 3, 0b01110000);
    }
    return 0;
}



