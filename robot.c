#include "robot.h"

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: initMSP430
// Purpose:  Initializes the MSP430's Timer A 0 for use with a 1ms interupt and the
//				ADC subsystem for use with pins 1.3, 1.4, 1.5
//--------------------------------------------------------------------------------

void initMSP430() {

	IFG1=0; 					// clear interrupt flag1
	WDTCTL=WDTPW+WDTHOLD; 		// stop WD

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	TA0CCR0 = PWM_FREQ;							// create a 1mS roll-over period
	TACTL &= ~TAIFG;							// clear flag before enabling interrupts = good practice
	TACTL = ID_3 | TASSEL_2 | MC_1 | TAIE;		// Use 1:8 presclar off MCLK and enable interrupts

	// ADC Subsystem Setup
	ADC10CTL0 = 0;									// Turn off ADC subsystem
	ADC10CTL1 = ADC10DIV_3;						// Channel 4, ADC10CLK/4
	ADC10AE0 = BIT4 | BIT3 | BIT5;		 								// Make P1.4, P1.3, and P1.2 analog inputs
	ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;		// Vcc & Vss as reference
	_enable_interrupt();

	return;
}

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: selectINCH
// Purpose:  Selects the in-channel for the ADC10 subsystem
//--------------------------------------------------------------------------------

void selectINCH(int8 inch){

	switch (inch){

	case 5:
		ADC10CTL0 &= ~ENC;
		ADC10CTL1 = INCH_5;
		ADC10CTL0 |= ENC;
		break;

	case 3:
		ADC10CTL0 &= ~ENC;
		ADC10CTL1 = INCH_3;
		ADC10CTL0 |= ENC;
		break;

	case 4:
		ADC10CTL0 &= ~ENC;
		ADC10CTL1 = INCH_4;
		ADC10CTL0 |= ENC;
		break;

	default:
		ADC10CTL0 &= ~ENC;
		ADC10CTL1 = INCH_5;
		ADC10CTL0 |= ENC;
		break;
	}

	return;
}

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: sleep
// Purpose:  Uses Timer A 0 to sleep for a specified number of millis.
//--------------------------------------------------------------------------------

void sleep(int16 millis){

	int16 i;
	TAR = 0;
	TACTL &= ~TAIFG;

	for(i = millis; i > 0; i--){
		while((TACTL & TAIFG) == 0);
		TACTL &= ~TAIFG;
		TAR = 0;
	}

	return;
}

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: shutDown
// Purpose:  Sets the motor terminals, enables on the driver chip low, and sets
//			 OUTMOD_5 for both motors which sets them low after one rollover.
//--------------------------------------------------------------------------------

void shutDown(){
	P2OUT &= ~(BIT0 | BIT5);
	P2OUT &= ~(BIT3 | BIT1);
	TA1CCTL1 = OUTMOD_5;
	TA1CCTL2 = OUTMOD_5;
	sleep(1);
}

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: initRobot
// Purpose:  Sets up the pins and Timer A 1 for use with the robot.
//--------------------------------------------------------------------------------

void initRobot(){
	// Initialize output pins
	    P2DIR |= BIT2;							// P2.2 is associated with TA1CCR1 and Right Motor
	    P2SEL |= BIT2;							// P2.2 is associated with TA1CCTL1

	    P2DIR |= BIT4;							// P2.2 is associated with TA1CCR2 and Left Motor
	    P2SEL |= BIT4;							// P2.2 is associated with TA1CCTL2

	    P2DIR |= BIT1;							// P2.1 and P2.3 are associated with the motor terminals
	    P2OUT &= ~BIT1;

	    P2DIR |= BIT3;
	    P2OUT &= ~BIT3;

	    P2DIR |= BIT0;							// P2.0 and P2.5 are associated with the motor enable and disable
	    P2OUT &= ~BIT0;

	    P2DIR |= BIT5;
	    P2OUT &= ~BIT5;

	    // Initialize the clock
		TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:1 presclar off MCLK
	    TA1CCR0 = PWM_FREQ;

	    TA1CCR1 = PWM_DUTY;
	    TA1CCTL1 = OUTMOD_5;					// set TACCTL1 to Reset mode (Grounded/Stopped)

	    TA1CCR2 = PWM_DUTY;
	    TA1CCTL2 = OUTMOD_5;
}

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: move and turn series
// Purpose:  moves the robot in the indicated way for a certain duration of millis
//--------------------------------------------------------------------------------

void moveForward(int16 duration){
	P2OUT &= ~(BIT3 | BIT1); 		// Motor terminal low for forward operation
	P2OUT |= BIT0 | BIT5;

	TA1CCTL1 = OUTMOD_7;
	TA1CCTL2 = OUTMOD_7;

	sleep(duration);

	shutDown();

	return;
}

void moveBackward(int16 duration){
	P2OUT |= BIT3 | BIT1; 		// Motor terminal high for backward operation
	P2OUT |= BIT0 | BIT5;

	TA1CCTL1 = OUTMOD_3;
	TA1CCTL2 = OUTMOD_3;

	sleep(duration);

	shutDown();

	return;
}

void turnRight(int16 duration){
	P2OUT |= BIT1;				//Right high -> Forward
	P2OUT &= ~ BIT3; 			//Left low -> Backward
	P2OUT |= BIT0 | BIT5;

	TA1CCTL1 = OUTMOD_7;
	TA1CCTL2 = OUTMOD_3;

	sleep(duration);

	shutDown();

	return;
}

void turnLeft(int16 duration){
	P2OUT |= BIT3;				//Left high -> Forward
	P2OUT &= ~ BIT1; 			//Right low -> Backward
	P2OUT |= BIT0 | BIT5;

	TA1CCTL1 = OUTMOD_3;
	TA1CCTL2 = OUTMOD_7;

	sleep(duration);

	shutDown();

	return;
}

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: accelerate series
// Purpose:  Speeds up the indicated motor by the amount parameter by modifying the duty cycle.
//--------------------------------------------------------------------------------
void accelerateRight(int16 amount){
	TA1CCR1 += amount;

	return;
}

void accelerateLeft(int16 amount){
	TA1CCR2 += amount;

	return;
}

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: resetSpeed
// Purpose:  returns the motor duty cycle to PWM_DUTY (which is 50%)
//--------------------------------------------------------------------------------

void resetSpeed(){
	TA1CCR1 = PWM_DUTY;
	TA1CCR2 = PWM_DUTY;

	return;
}

//--------------------------------------------------------------------------------
// Author:   C2C Bodin
// Function: hugWallR and hugWallL
// Purpose:  Moves the robot forward while maintaining a set distance from the wall.
//				wallDistance should be the global variable carrying the appropraiate
//				ADC sensor coversion.
//--------------------------------------------------------------------------------

void hugWallR(int16 wallDistance){
	if((wallDistance > (DIST_THRESHOLD_R - SENSOR_NOISE)) && (wallDistance < (DIST_THRESHOLD_R + SENSOR_NOISE))){
		resetSpeed();
		moveForward(10);
	}

	if(wallDistance > (DIST_THRESHOLD_R + SENSOR_NOISE)){
		accelerateRight(100);
		moveForward(10);
	}

	if(wallDistance < (DIST_THRESHOLD_R - SENSOR_NOISE)){
		accelerateLeft(100);
		moveForward(10);
	}

	return;
}

void hugWallL(int16 wallDistance){
	if((wallDistance > (DIST_THRESHOLD_L - SENSOR_NOISE)) && (wallDistance < (DIST_THRESHOLD_L + SENSOR_NOISE))){
		resetSpeed();
		moveForward(10);
	}

	if(wallDistance > (DIST_THRESHOLD_L + SENSOR_NOISE)){
		accelerateLeft(100);
		moveForward(10);
	}

	if(wallDistance < (DIST_THRESHOLD_L - SENSOR_NOISE)){
		accelerateRight(100);
		moveForward(10);
	}

	return;
}
