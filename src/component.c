#include "component.h"

LED_Matrix init_led_matrix(LED_Matrix *matrix, GPIO_TypeDef* gpio, int layer_pins[Matrix_length]) {
    matrix->gpio = gpio;
    matrix->update_rate = deafult_update_rate;
    matrix->intensity = deafult_intensity;

    for(int i = 0; i < Matrix_length; i++){
        for(int j = 0; j < Matrix_length; j++){
            for(int k = 0; k < Matrix_length; k++){
                matrix->lighten_coordinate[i][j][k] = 0;
            }
        }
    }

    for(int i = 0; i < Matrix_length; i++){
		matrix->layer_pins[i] = layer_pins[i];
		init_led(gpio, layer_pins[i]);
        matrix->lighten_layer[i] = 0;
	}

    return *matrix;
}

MAX7219 init_max7219(MAX7219 *max7219_interface, GPIO_TypeDef *gpio, int DIN, int CS, int CLK) {
    // Enable AHB2 Clock
    if (gpio == GPIOA)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    else if (gpio == GPIOB)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    else if (gpio == GPIOC)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // Set GPIO pins to output mode (01)
    // First Clear bits(&) then set bits(|)
    gpio->MODER &= ~(0b11 << (2 * DIN));
    gpio->MODER |= (0b01 << (2 * DIN));
    gpio->MODER &= ~(0b11 << (2 * CS));
    gpio->MODER |= (0b01 << (2 * CS));
    gpio->MODER &= ~(0b11 << (2 * CLK));
    gpio->MODER |= (0b01 << (2 * CLK));

    max7219_interface->gpio = gpio;
    max7219_interface->DIN = DIN;
    max7219_interface->CS = CS;
    max7219_interface->CLK = CLK;

    // Set Decode Mode to non-decode mode
    send_max7219(max7219_interface, MAX_ADDRESS_DECODE_MODE, 0x00);
    // Set Intensity to Highest Intensity
    send_max7219(max7219_interface, MAX_ADDRESS_ITENSITY, 0xFF);
    // Set Scan Limit 
    send_max7219(max7219_interface, MAX_ADDRESS_SCAN_LIMIT, 0x07);
    // Wakeup max7219
    send_max7219(max7219_interface, MAX_ADDRESS_SHUTDOWN, 0x01);
    // Close display test
    send_max7219(max7219_interface, MAX_ADDRESS_DISPLAY_TEST, 0x00);

    return *max7219_interface;
}

void send_max7219(MAX7219 *max7219_interface, int address, int data)
{
    // The payload to send
    int payload = ((address & 0xFF) << 8) | (data & 0xFF);

    // Start the sending cycles
    // 16 data-bits + 1 CS signal
    int total_cycles = 16 + 1;

    for (int a = 1; a <= total_cycles; a++)
    {
        // Reset CLK when enter
        reset_gpio(max7219_interface->gpio, max7219_interface->CLK);

        // Set DIN according to data except for last cycle(CS)
        if (((payload >> (total_cycles - 1 - a)) & 0x1) && a != total_cycles)
            set_gpio(max7219_interface->gpio, max7219_interface->DIN);
        else
            reset_gpio(max7219_interface->gpio,max7219_interface->DIN);

        // Set CS at last cycle
        if (a == total_cycles)
            set_gpio(max7219_interface->gpio, max7219_interface->CS);
        else
            reset_gpio(max7219_interface->gpio, max7219_interface->CS);

        // Set CLK when leaving (7seg set data at rising edge)
        set_gpio(max7219_interface->gpio, max7219_interface->CLK);
    }
    return;
}

int read_gpio(GPIO_TypeDef* gpio, int pin){
	return (gpio->IDR >> pin) & 1;
}
void write_gpio(GPIO_TypeDef* gpio, int pin, int data){
	gpio->BSRR |= (data << pin);
}
void set_gpio(GPIO_TypeDef* gpio, int pin){
	gpio->BSRR |= (1 << pin);
}
void reset_gpio(GPIO_TypeDef* gpio, int pin){
	gpio->BRR |= (1 << pin);
}
void toggle_output(GPIO_TypeDef* gpio, int pin){
	if(read_gpio(gpio, pin)==0){
		set_gpio(gpio, pin);
	}
	else{
		reset_gpio(gpio, pin);
	}
}

void choose_layer(LED_Matrix* matrix, int layer, int state){
    for(int i = 0; i < Matrix_length; i++)
        reset_gpio(matrix->gpio, matrix->layer_pins[i]);
    if(state)
        set_gpio(matrix->gpio, matrix->layer_pins[layer]);
}

void delay_without_interrupt(int msec){
	int loop_cnt = 500*msec;
	while(loop_cnt){
		loop_cnt--;
	}
	return;
}

// Only allow GPIOA and GPIOB for now
// Can easily extended by adding "else if" cases
void init_led(GPIO_TypeDef* gpio, int LED_pin){
    // Enable AHB2 Clock
    if (gpio == GPIOA)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    else if (gpio == GPIOB)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    else if (gpio == GPIOC)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// Set GPIO pins to output mode (01)
	// First Clear bits(&) then set bits(|)
	gpio->MODER &= ~(0b11 << (2*LED_pin));
	gpio->MODER |= (0b01 << (2*LED_pin));
}

// Only allow GPIOC for now
// Can easily extended by adding "else if" cases
int init_button(GPIO_TypeDef* gpio, int button_pin){
	// Enable AHB2 Clock
	if(gpio==GPIOC){
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	}
	else{
		// Error! Add other cases to suit other GPIO pins
		return -1;
	}

	// Set GPIO pins to input mode (00)
	// First Clear bits(&) then set bits(|)
	gpio->MODER &= ~(0b11 << (2*button_pin));
	gpio->MODER |= (0b00 << (2*button_pin));

	return 0;
}

// A simple way to read on board button
int user_press_button(int debounce){
	int cnt=0;
	for(int a=0;a<debounce;a++){
		if(read_gpio(GPIOC, 13)==0){
			cnt++;
		}
	}
	// Use 0.5 for threshold just to make it easier
	if(cnt>(debounce>>1)){
		return 1;
	}
	return 0;
}


