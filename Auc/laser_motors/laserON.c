#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>

#define LASER_PIN 4

#define DIR_X 18
#define STEP_X 17

int main(){

	if (wiringPiSetupGpio() == -1){
		return 1;
	}
	puts("K");
	pinMode(DIR_X, LOW);
	pinMode(STEP_X,OUTPUT);
	pinMode(LASER_PIN,OUTPUT);
	digitalWrite(LASER_PIN,HIGH);
	digitalWrite(DIR_X,HIGH);
	while(1){
		digitalWrite(STEP_X,HIGH);
		delayMicroseconds(50);
		digitalWrite(STEP_X,LOW);	
		delayMicroseconds(50);
	}
	
	sleep(10);
	return 0;
}
