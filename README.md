# About

This is the final project for Microprocessor_2023, my partner @chrispies181 and I build a 5x5x5 LED cube.
![LED_cube](/asset/image4.jpg)
![Animation](/asset/LED_cube_collide_compressed.gif)

# Hardware

## Material List

- STM32L476RG x 1
![stm32l476rg](/asset/image6.jpg)
- yellow LEDs x 125
- 2n2222a transistor x 25
- MAX7221 Led display drivers x 1
![max7219](/asset/image3.jpg)

## Steps

1. Welding the led matrix:
    - To bend the pins of LEDs and arrange them from a row to a layer, and then assemble them into an LED cube.
    - In each layer, the positive pins connect with each other, the negative pins connect vertically with the LEDs of the top and bottom side from the other layers.
    - Make sure to the LEDs in the same layer are working properly before welding the next layer.
    ![single LED](/asset/image7.png)
    ![single layer](/asset/image8.png)
    - [LED cube](/asset/image9.png)

2. Welding the transistor:
    - Using collector and base pins of the transistor to build a layer, take collector as the positive led pin and base as the negative led pin.
    - Connect emitter pins to the bottom layer of the LED cube vertically.
    ![transistor layer](/asset/image5.jpg)

## Schematic

The most common way to control the LED cube is to use one MAX7219 for each layer (five max7219 for five layers), however we found that if we control the current flow by adding the transistor to the pins of max7219, we can use only one max7219 to control all the layers.

- Single LED layer  
![single led layer](/asset/image13.png)
- Max7219 layer  
![max7219 layer](/asset/image12.png)
- Circuit diagram  
![circuit diagram](/asset/image14.png)

# Firmware

## Components

- In [component.h](/include/component.h) we define the pins of the LED cube, including the LED cube itself, the MAX7219, and the STM32L476RG. If change the pins of the LED cube, you need to change the pins in this file.

- The struct LED_Matrix in [component.h](/include/component.h) is used to store the status of the LED cube at instant, including:
    - the pins
    - update_rate
    - brightness
    - the lighten corrdinate
        - key: the x, y, z coordinate of the LED
        - value: 0 - off, 1 - on
    - the lighten layer
        - key: the layer of the LED cube
        - value: 0 - off, 1 - on
- If you want to change the pattern of the LED cube, you need to change the struct LED_Matrix which will be used in the [main.c](/src/main.c).

- In [animation.h](/include/animation.h), we define how the LED cube works, it is not necessary to understand the code in this file, but you still can control the LED cube by change the struct LED_Matrix and import it into the display function.

# Display control

```c
void display(MAX7219* max7219_interface, LED_Matrix* led_matrix);
```

## Example

- In this example, we simulate a ball collision in the LED cube. The lighten LED represent the position the ball in the LED cube.
- We use the struct Point in [animation.h](/include/animation.h) to store the position and vector of the ball, and using the [colliding()](/src/animation.c) to update the ball status. When the ball hitthe wall, it will bounce back base on it vector.

```c
Point new_point = colliding(point);
test_matrix.lighten_coordinate[(int)(point.x_pos)][(int)(point.y_pos)][(int)(point.z_pos)] = 1;
display(&max7219_interface, &test_matrix);
point = new_point;
```
