#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE * device;   //pointer for a file handled to driver

//Push function 
void push(char * status){
	device = fopen("/dev/arduino1","r+");
	if(device != NULL){
		if (strcmp(status, "ON") == 0) {
			fprintf(device, "%c", 'p');
		}
		else if (strcmp(status, "OFF") == 0){
			fprintf(device, "%c", 'P');
		}
		fclose(device);
	}
	else{
		printf("device not found\n");
	}
}

//Touch function 
void touch(){
	device = fopen("/dev/arduino1","r+");
	if(device != NULL){
		fprintf(device, "%c", 't');
		fclose(device);
	}
	else{
		printf("device not found\n");
	}
}

//Move function, receive an appropiate new position for the movement
void move(int position){
	device = fopen("/dev/arduino1","r+");
	if(device != NULL){
		fprintf(device, "%d", position);
		fclose(device);
	}
	else{
		printf("File no found\n");
	}
}





