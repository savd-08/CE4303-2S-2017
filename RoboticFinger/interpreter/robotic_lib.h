#ifndef _LIBRARY_H
#define _LIBRARY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define M_PI  3.14159265358979323846
#define GEAR_D  56

FILE * device;   
char *port;

typedef struct point3d {
	uint8_t x;
	uint8_t y;
	uint8_t z;
  } point;

int set_port(char *port_a);
void push(uint8_t time);
void touch();
void move(float pos_x, float pos_y);
int mm2deg(float mm);

#endif

