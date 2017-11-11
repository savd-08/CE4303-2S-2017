#include "robotic_lib.h"

//Set port file
int set_port(char *port_a){
	//Open port file
	device = fopen(port_a, "r+");
	if (!device)
	{
		//Returns false if failed
		printf("Error opening port\n");
		return 0;
	} 
	else
	{
		//Set port if successful
		fclose(device);
		port = port_a;
		return 1;
	}
}

//  Converts mm to degrees for the servos
int mm2deg(float mm) {
	return mm/M_PI/GEAR_D*360;
  }

//Press the screen
void push(uint8_t time){
	//Open port file
	device = fopen(port,"r+");
	//Sets z axis to touch the screen
	point p = {.x = 0, .y = 0, .z = 120};
	if(device != NULL){
		//Send coordinates 
		fwrite((const void *) &p, sizeof(point), 1, device);
		//Delay and move finger up
		sleep(time);
		p.x = 1; 
		p.y = 1; 
		p.z = 0;
		fwrite((const void *) &p, sizeof(point), 1, device);
		fclose(device);
	}
	else
	{
		printf("I/O Error\n");
	}
}

//Taps the screen
void touch(){
	//Open port file
	device = fopen(port,"r+");
	//Sets z axis to touch the screen
	point p = {.x = 0, .y = 0, .z = 120};
	if(device != NULL){
		//Send coordinates 
		fwrite((const void *) &p, sizeof(point), 1, device);
		//Very small delay and move finger up
		usleep(50000);
		p.x = 1; 
		p.y = 1; 
		p.z = 0;
		fwrite((const void *) &p, sizeof(point), 1, device);
		fclose(device);
	}
	else
	{
		printf("I/O Error\n");
	}
}

//Moves the finger to the given coordinates
void move(float pos_x, float pos_y){
	//Open port file
	device = fopen(port,"r+");
	//Set X,Y coordinates to move the finger to
	point p = {.x = mm2deg(pos_x), .y = mm2deg(pos_y), .z = 0};
	if(device != NULL){
		//Send coordinates
		fwrite((const void *) &p, sizeof(point), 1, device);
		fclose(device);
	}
	else 
	{
		printf("I/O Error\n");
	}
}