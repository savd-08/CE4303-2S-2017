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
point_struct * actual_pos;

int struct_size = (int) sizeof(struct point);

/*
//  Gear settings
float gearD = 56;
float ZgearD = 13.3239;*/

/*
//  Drawing robot limits, in degrees
float Xdmin = 17;   //  8.31mm
float Xdmax = 171;  //  83.57mm
float Ydmin = 25;   //  Functionally 18 degrees  //  12.22mm
float Ydmax = 146;  //  Functionally 146        //  71.35mm
float Zdmin = 18;
float Zdmax = 50;

//  Drawing robot limits, in mm
float Xmin = deg2mm(Xdmin);
float Xmax = deg2mm(Xdmax);
float Ymin = deg2mm(Ydmin);
float Ymax = deg2mm(Ydmax);
float Zmin = Zdmin;
float Zmax = Zdmax;*/

uint8_t Xpos = 0;
uint8_t Ypos = 0;
uint8_t Zpos = 0;

/**********************
 * void setup() - Initialisations
 ***********************/
void setup() {
  //  Setup
  Serial.begin( 115200 );
  //  Attach servos
  servoX.attach( SERVOPINX );
  servoY.attach( SERVOPINY );
  servoZ.attach( SERVOPINZ );
  //  Set & move to initial default position
  servoX.write(Xpos);
  servoY.write(Ypos);
  servoZ.write(Zpos);
  //  Notifications!!!
  /*Serial.println(" degrees");
  Serial.print("X range is from ");
  Serial.print(Xmin);
  Serial.print(" to ");
  Serial.print(Xmax);
  Serial.println(" mm.");
  Serial.print("Y range is from ");
  Serial.print(Ymin);
  Serial.print(" to ");
  Serial.print(Ymax);
  Serial.println(" mm.");*/
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
    if(actual_pos->z == 0 && actual_pos->x != 1 && actual_pos->y != 1){
      Xpos = actual_pos->x;
      Ypos = actual_pos->y;
      Zpos = actual_pos->z;
      writeServo();
    } else if(actual_pos->x == 1 && actual_pos->y == 1) {
      Zpos = actual_pos->z;
      writeServo();
    }

  }

}

/******************************************************
Function for moving the figner
******************************************************/

void writeServo(){
  servoX.write(Xpos);
  servoY.write(Ypos);
  servoZ.write(Zpos);
}


//  Converts mm to degrees for the servos
/*float mm2deg(float mm) {
  return mm/PI/gearD*360;
}
//  Converts mm to degrees for the servos
float deg2mm(float deg) {
  return PI*gearD*deg/360;
}*/
