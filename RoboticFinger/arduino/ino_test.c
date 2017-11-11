#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

FILE * device;

/* Structures, global variables */
typedef struct point {
  uint8_t x;
  uint8_t y;
  uint8_t z;
} point_struct;

int main(int argc, char const *argv[]) {
  device = fopen("/dev/arduino0","r+");

  point_struct point_s;

  point_s.x = 40;
  point_s.y = 40;
  point_s.z = 0;

  if(device != NULL){

      fwrite((const void *) &point_s, sizeof(point_struct), 1, device);

      fclose(device);
  }

  return 0;
}
