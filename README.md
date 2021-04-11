# LCDI2C-STM32F103RBT6

This repository contains drivers for LCD display 16x2 characters with soldered converter PCF8574 for I2C bus. Based on provided inputs in Init function, the drivers can by itself initialize display, display strings, floating and integer numbers, display and set a cursor and clear the display space Displaying data is possible with formatting to the right side of designated space

The drivers are designed for NUCLEO-F103RB evalutation board with STM32F103RBT6 64 pins ARM Cortex-M3 core microcontroller. It requires HAL library for proper working