/*
 * robot.h
 * Author: Taylor Bodin
 * Date: 18 Nov 2014
 * Description: Prototypes robot control functions
 */
#include <msp430g2553.h>

typedef		unsigned char		int8;
typedef		unsigned short		int16;
typedef		unsigned long		int32;
typedef		unsigned long long	int64;

#define PWM_FREQ 0x0BA2
#define PWM_DUTY 0x05D1

#define 	LEFT 	1
#define 	CENTER 	2
#define 	RIGHT 	3

#define		DIST_THRESHOLD_L	0x128
#define		DIST_THRESHOLD_R	0x13A
#define		DIST_THRESHOLD_C	0x164
#define		SENSOR_NOISE		0x018	//Average SD of some sensor readings. I've multiplied by 3 to be 99% sure.

#define		TRUE				1
#define		FALSE				0

void initMSP430();

void selectINCH(int8 inch);

void sleep(int16 millis);

void shutDown();

void initRobot();

void moveForward(int16 duration);

void moveBackward(int16 duration);

void turnLeft(int16 duration);

void turnRight(int16 duration);

void accelerateRight(int16 amount );

void accelerateLeft(int16 amount);

void resetSpeed();

void hugWallR(int16 wallDistance);

void hugWallL(int16 wallDistance);
