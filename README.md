ECE382_Lab8
===========

Maze solving robot

## Objectives and Purpose

Combined Lab 6 and 7 functionalities to create a maze solving robot

## Functions 

Note that, in the intrest of brevity and clarity, I'll only discuss new or updated functions. All other functions can be
found in the Lab 6 and 7 write ups. 

### acclerateRight and accelerateLeft

```
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
}
```

These two functions work by modifying TA1CCR1 and TA1CCR2 which control the duty cycle in the PWM subsystem. It turns out
that a modification of 100 works really well when you update the speed every 10ms or so. 

### resetSpeed

```
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
```

Acting as accelerate's antagonist, reset speed resets the PWM duty cycle to PWM_DUTY which is defined to be
50%. 

### hugWallL and hugWallR

```
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

```
These two little functions are probably my favorite of the year. I only intended them to allow me to travel straight down a
hallway in the maze, but during my testing I found that they would reliably follow a wall AROUND A CORNER! This was a very 
handy function to have to solve the maze. In its orginal form it was written to just follow a right wall, but it's much better
in this particular maze to hug the left side since the left wall is always connected to the exit with nice long sections, vice
the part of the right wall that just juts out. I copied the original function under itself and changed it to work with the
left wall. I don't actually use hugWallR in main but I thought I would keep it around. It's function is fairly simple. Every 10ms
these functions check the distance to the wall through wallDistance. If it's too far away it speeds up the outside motor, and
if it's too close it speeds up the inside motor. To make the course corrections dynamically stable, I added a range to keep 
the robot within and reset the motor speeds when I was within this range. Note that SENSOR_NOISE is 3 standard deviations
so that, theoretically, I'm 99.7% confident that I'm within my specified range. 

### main

```
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
```

This is main. The bulk of the code is broken into two part, initilization and a variation of the wall follower algorithm for
solving mazes. The reason I added apparently to my comment above the algorithm was because I stumbled upon it by accident.
The algorithm is fairly simple and widely known. Follow the wall until you hit a dead end, turn 90 degrees right, and continue
to follow the same wall until you hit another dead end or you're out. The logic statement in the while loop detects dead ends,
hugWallL follows a the wall, and turnRight(250) is a pretty consistent 90 degree turn. 

## Debugging and Testing

My MSP430 is very finicky when plugged into the robot and my computer at the same time which made debugging difficult. The
primary probleme I ran into was that my sensors would not read correctly when I was plugged into my computer but would when
it was operating in isolation. After many hours of frustration, I learned to trust my robot and test only isolation. Another
hurdle I ran into was confusing Timer A 1 and Timer A 0 in my code. When I started to implement things, I pretty much
grabbed and dropped sections of code from my old labs right in the project. I had decide which functions were going to use
which timers and how I was going to set up those timers. What I settled on was TA0 controlled the ISR and sleep functions,
which required slight modifications to sleep, and TA1 controlled the PWM signals I was sending to my robot. At this point
I had a moving robot with working sensors. My testing from then on was trial and error. Once I discovered my hugWall function
could follow around corner I changed my implementation to follow the left wall to make that first turn. This gave me required
functionality. Then I changed my code to execute a right 90 degree turn and start following the left wall again. By sheer luck,
my robot completed the whole maze and I realized that I had stumbled on the wall follower algorithm. 

## Conclusion

I really liked this class and I will be sad to leave James Bond (Robot #007) behind. There are still a few
kinks, especially with detecting a dead end, but since the algorithm works on this type of maze, it is self correcting and 
will eventually find its way out which is super cool. I'm very proud of my 3ish lines of code in main solving all three
functionalities, but I realize that many of my discoveries were made by accident when I was banging my head on the wall.
That's a really cool thing about programming that I'll take with me. 

## Documentation
NONE
