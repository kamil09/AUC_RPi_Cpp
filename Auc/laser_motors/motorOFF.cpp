#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>

#define MOTOR_ENABLE 25

int main(){
	if (wiringPiSetupGpio() == -1){
		return 1;
	}
	pinMode(MOTOR_ENABLE,OUTPUT);
	digitalWrite(MOTOR_ENABLE,HIGH);
}
