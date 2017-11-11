/******************************************************
CNC Program, move the finger in the 2 axis, and rise and
drop a finger
******************************************************/

#include <Servo.h>

#define SERVOPINX 11
#define SERVOPINY 10
#define SERVOPINZ 9
#define LINE_BUFFER_LENGTH 512

/* Structures, global variables */
typedef struct point {
  uint8_t x;
  uint8_t y;
  uint8_t z;
} point_struct;

/* Naming servos */
Servo servoX;
Servo servoY;
Servo servoZ;

/* actual point */
point_struct *actual_pos;

int struct_size = (int) sizeof(struct point);

uint8_t Xpos = 0;
uint8_t Ypos = 0;
uint8_t Zpos = 0;

/**********************
 * void setup() - Initialisations
 ***********************/
void setup() {
  //  Setup
  Serial.begin(9600);
  //  Attach servos
  servoX.attach( SERVOPINX );
  servoY.attach( SERVOPINY );
  servoZ.attach( SERVOPINZ );
  //  Set & move to initial default position
  servoX.write(Xpos);
  servoY.write(Ypos);
  servoZ.write(Zpos);
}

/******************************************************
Serial read and moving determination
******************************************************/

void loop(){

  if(Serial.available()){
    //in buffer
    char serial_buffer [struct_size];
    //Serial read
    Serial.readBytes(serial_buffer, struct_size);
    //convertion to a point struct
    actual_pos = (point_struct*) &serial_buffer;
    //move the finger
    Xpos = actual_pos->x;
    Ypos = actual_pos->y;
    Zpos = actual_pos->z;
    servoX.write(Xpos);
    servoY.write(Ypos);
    servoZ.write(Zpos);
    //writeServo();
    delay(1000);
  }
}

