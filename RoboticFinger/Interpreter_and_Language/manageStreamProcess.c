/*
 * manageStreamProcess.c
 *
 */

#include "libRoboticFinger.h"

/*
 *
 * File that receives the values from the parser and sends that values to the
 * Device Driver Library.
 *
 */

 char* operationToProcess;
 int proPosition;
 int pos_X1;
 int pos_Y1;
 int pos_X2;
 int pos_Y2;
 int time;
 extern int screenSize;

/*
 * Defines what type of instructions are being processed by the parser
 */

 void defineOperationType(char * operationType){
 	operationToProcess = operationType;
 	proPosition = 0;
	time = 0;
 	pos_X1 = 0;
 	pos_Y1 = 0;
 	pos_X2 = 0;
 	pos_Y2 = 0;
 }

/*
 * Function that sends the processed instructions to the Device Library
 */

//Revisar tiempos
void sendToDriver(){
	if(strstr(operationToProcess,"touch")  ) {
		move(pos_X1,pos_Y1,screenSize);
		if(screenSize == 3)
			usleep(3000000);
		else
			usleep(2300000);
		press();
		usleep(1020000);
		release();
		usleep(2300000);
		init();
		usleep(1020000);
		printf("DONE\n");
	}
	else if(strstr(opToProcess,"drag")  ) {
		move(pos_X1,pos_Y1,sizeScreen);
		if(sizeScreen == 4)
			usleep(3000000);
		else
			usleep(2300000);
		press();
		usleep(1020000);
		release();
		usleep(1020000);
		shift(pos_X2, pos_Y2,sizeScreen);
		usleep(2300000);
		init();
		usleep(1020000);
		printf("DONE\n");
		}
	else if(strstr(operationToProcess,"push")  ) {
		if(time == 0 || time == 1){
			move(pos_X1,pos_Y1,sizeScreen);
			usleep(2300000);
			press();
			usleep(1020000);
			release();
			usleep(2300000);
			init();
			usleep(1020000);
			printf("DONE\n");
		}
		else {
			move(pos_X1,pos_Y1,sizeScreen);
			usleep(2300000);
			press();
			sleep(time);
			release();
			usleep(2300000);
			init();
			usleep(1020000);
			printf("DONE\n");
		}
	}
}

/*
 * Receives the position values from the parser depending of the type of
 * instruction.
*/

void managePosition(int position){
	if(strstr(operationToProcess,"touch")  ) {
		processTouch(position);
	}
	else if(strstr(operationToProcess,"drag")  ) {
			processDrag(position);
	}
	else if(strstr(operationToProcess,"push")  ) {
		processPush(position);
	}
}

/*
 * Function that receives the values from the parser when the instructions
 * are of Touch type
 */

void processTouch(int position){
	switch (proPosition) {
			case 0:
				pos_X1 = position;
				proPosition++;
				break;
			case 1:
				pos_Y1 = position;
				proPosition++;
				break;
		}
}

/*
 * Function that receives the values from the parser when the instructions
 * are of Drag type
 */

void processDrag(int position){
	switch (proPosition) {
				case 0:
					pos_X1=position;
					proPosition++;
					break;
				case 1:
					pos_Y1=position;
					proPosition++;
					break;
				case 2:
					pos_X2=position;
					proPosition++;
					break;
				case 3:
					pos_Y2=position;
					proPosition++;
					break;
			}
}

/*
 * Function that receives the values from the parser when the instructions
 * are of Push type
 */

void processPush(int position){
	switch (proPosition) {
			case 0:
				pos_X1 = position;
				proPosition++;
				break;
			case 1:
				pos_Y1 = position;
				proPosition++;
				break;
			case 2:
				time = position;
				proPosition++;
				break;
		}
}
