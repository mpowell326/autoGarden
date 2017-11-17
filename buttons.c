/*
 * button.c
 *
 *  Created on: Apr 11, 2015
 *      Author: Ran
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pwm.h"
#include "inc/hw_ints.h"
#include "inc/lm3s1968.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "driverlib/debug.h"
#include "button.h"
#include "constant.h"

volatile unsigned long butIntCount; // Monitors interrupts
extern volatile unsigned long sysTick;

void buttonsInit(void)
{
    // regsiter the handler for port G into the vector table
    GPIOPortIntRegister (GPIO_PORTB_BASE, ButtonIntHandler);

    // enable the PG3 to PG7 to read the five buttons
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4, GPIO_DIR_MODE_IN);

    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4, GPIO_STRENGTH_2MA,
                         GPIO_PIN_TYPE_STD_WPU);

    // setup the pin change interrupt
    GPIOIntTypeSet (GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4, GPIO_FALLING_EDGE);

    // enable the pin change interrupt
    GPIOPinIntEnable (GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4);


IntEnable (INT_GPIOB);