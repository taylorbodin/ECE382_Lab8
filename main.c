//-----------------------------------------------------------------
// Name:	Bodin
// File:	main.c
// Date:	Fall 2014
// Purp:	To solve the ECE382 maze reliably.
//-----------------------------------------------------------------
#include "msp430g2553.h"
#include "robot.h"

int16 leftDistance;
int16 rightDistance;
int16 centerDistance;
int8 lastSensor;

int main(void) {

	IFG1=0; 									// clear interrupt flag1
	WDTCTL = WDTPW + WDTHOLD;					// disable WDT

	initMSP430();
	initRobot();

	lastSensor = 4;								// throwaway value to kickoff ADC conversions


	//The code below implements a variation of the wall follower algorithm apparently.
	while(1){

		while(centerDistance < (DIST_THRESHOLD_C + SENSOR_NOISE)){
			hugWallL(leftDistance);
		}

		turnRight(250);
	}
} // end main

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: ADC10 ISR
// Purpose:  updates a sensor global variable every 1ms and starts the next sensor conversion
//--------------------------------------------------------------------------------
#pragma vector = TIMER0_A1_VECTOR				// This is from the MSP430G2553.h file
__interrupt void timerOverflow (void) {

	while(ADC10CTL1 & ADC10BUSY);

	switch ( lastSensor ){

	case LEFT:
		leftDistance = ADC10MEM;
    	selectINCH(4);
    	lastSensor = CENTER;
    	break;

	case CENTER:
		rightDistance = ADC10MEM;
    	selectINCH(3);
    	lastSensor = RIGHT;
    	break;

	case RIGHT:
		centerDistance = ADC10MEM;
		selectINCH(5);
		lastSensor = LEFT;
		break;

	default:
		selectINCH(5);
		ADC10CTL0 |= ADC10SC;
		lastSensor = CENTER;
		break;
	}

		ADC10CTL0 |= ADC10SC;

	    TACTL &= ~TAIFG;
}
