//*****************************************************************************
//
//
// Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the EK-LM4F120XL Firmware Package.
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"

#define GPIO_PB6_T0CCP0         0x00011807 //Added this because pin mapping from pin_map.h not recognized by default


unsigned volatile long pwmPeriod = 1000; //Period for Timer 0 PWM. Note: For 16-bit timer, max count is 65535.
unsigned volatile long dutyCycle = 500; //50% of pwmPeriod


unsigned volatile long count = 0; //To count the number of seconds
unsigned volatile long pulse = 0; //To count the number of PWM pulses
unsigned volatile int runPWM = 0; //To initialize PWM
unsigned volatile int increment = 0; //To increment/decrement PWM pulse width

//unsigned volatile int change = 0; //for toggling LED

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// The interrupt handler for Timer 0 interrupt.
// Not used for this example
//*****************************************************************************
void
Timer0IntHandler(void)
{
		
	

}

//*****************************************************************************
//
// The interrupt handler for Timer 1 interrupt.
//
//*****************************************************************************
void
Timer1IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Modification 1a on 10_02_2014
    // Toggle the LED
    //
	/*if (change == 0)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
		change = 1;
	}
	if (change == 1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
		change = 0;
	}*/
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)^(0x04)));
    //End Modification 1a on 10_02_2014
	
	
	// Configuring timer0 for pwm operation
	// Modification 1b on 9_29_2014
	if (count == 20)
	{
		//TimerDisable(TIMER0_BASE, TIMER_A);
		pwmPeriod = 667; 
		dutyCycle = 167;
		TimerLoadSet(TIMER0_BASE, TIMER_A, pwmPeriod); //24kHz
		TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycle); //75% duty cycle
		//TimerEnable(TIMER0_BASE, TIMER_A);
	}
	if (count == 40)
	{
		//TimerDisable(TIMER0_BASE, TIMER_A);
		pwmPeriod = 1600; 
		dutyCycle = 1000;
		TimerLoadSet(TIMER0_BASE, TIMER_A, pwmPeriod); //10kHz
		TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycle); //25% duty cycle
		//TimerEnable(TIMER0_BASE, TIMER_A);
	}	
	
	if (count == 60)
	{
		runPWM = 1;
		timerPeriod = SysCtlClockGet() / 1000; // raises 100 interrupts in a second to trigger change in pulse width
		TimerLoadSet(TIMER1_BASE, TIMER_A, timerPeriod);
		
	}
	
	if (count > 60)
	{
		// perform operation
		if (runPWM == 1)
		{
			if (dutyCycle >= 1600)
			{
				increment = 0;
			}
			
			if (dutyCycle <= 0)
			{
				increment = 1;
			}
			
			if (increment == 1)
			{
				dutyCycle+= 32;
			}
			else
			{
				dutyCycle-= 32;
			}
			TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycle);
			
		}
	}
	
	
	
	if (count == 2000)
	{
		runPWM = 0;
		timerPeriod = SysCtlClockGet() / 2;
		TimerLoadSet(TIMER1_BASE, TIMER_A, timerPeriod);		
		//TimerDisable(TIMER0_BASE, TIMER_A);
		pwmPeriod = 1000; 
		dutyCycle = 500;
		TimerLoadSet(TIMER0_BASE, TIMER_A, pwmPeriod); //16kHz
		TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycle); //50% duty cycle
		//TimerEnable(TIMER0_BASE, TIMER_A);
		count = 0;
	}
	count ++;
	//End Modification 1b on 9_29_2014

}


int main(void)
{

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable the GPIO pin for the LED (PF2).  
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04); //Turn on LED
	
    //Modification 1b on 9_29_2014
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); //Enable Timer 1 peripheral
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC_UP); //Regular 32-bit timer in timer periodic mode
	unsigned long timerPeriod = SysCtlClockGet() / 2; //System clock is at 16MHz and timerPeriod is set to 8000000 here; Toggles twice a second
	//End Modification 1b on 9_29_2014
	
	TimerLoadSet(TIMER1_BASE, TIMER_A, timerPeriod); //Load the timer with the required period
	
    //
    // Setup the interrupt for the timer timeout
    //
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); //Enable Timer 0 peripheral
	TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM); //Split to make it two 16-bit timers and run in PWM mode
	
	//
	// Enable the GPIO port that is used for timer 0 PWM mode output
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB6_T0CCP0); //Timer 0 CCR can be routed to GPIO pin PB6
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);

    //Timer 0 counts down from pwmPeriod and pin stays high until it reaches 'dutyCycle' at which point it toggles low. Toggles back to high when it reaches 0 and process repeats.
    TimerLoadSet(TIMER0_BASE, TIMER_A, pwmPeriod); //16kHz
    TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycle);
	
	
	
	//
    // Enable processor interrupts.
    //
    IntMasterEnable();
	
    //
    // Enable the timers.
    //
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_A);

    //
    // Loop forever while the timers run.
    //
    while(1)
    {
	
		
	}
}
