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
struct point {
  float x;
  float y;
  float z;
};

/* Naming servos */
Servo servoX;
Servo servoY;
Servo servoZ;

/* actual point */
struct point actuatorPos;

//  Gear settings
float gearD = 56;
float ZgearD = 13.3239;

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
float Zmax = Zdmax;

float Xpos = Xdmin;
float Ypos = Ydmin;
float Zpos = Zmax;

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
  Serial.println(" degrees");
  Serial.print("X range is from ");
  Serial.print(Xmin);
  Serial.print(" to ");
  Serial.print(Xmax);
  Serial.println(" mm.");
  Serial.print("Y range is from ");
  Serial.print(Ymin);
  Serial.print(" to ");
  Serial.print(Ymax);
  Serial.println(" mm.");
}

void loop(){
}

/******************************************************
Functions for Calculating Useful Stuff
******************************************************/
//  Converts mm to degrees for the servos
float mm2deg(float mm) {
  return mm/PI/gearD*360;
}
//  Converts mm to degrees for the servos
float deg2mm(float deg) {
  return PI*gearD*deg/360;
}
