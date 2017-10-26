#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_LENGTH 256
static char receive[BUFFER_LENGTH];
char message[10];
int Device;
int Obtained;
char stringToSend[BUFFER_LENGTH];

void move(int posX, int posY, int sizeMatrix); // send message to move in x or y axis
void shift(int posX, int posY, int sizeMatrix); // send drag movement
void press(); // send's message to press
void release(); // send's message to release
void init(); // send's message to reverse to its initial position
void high(); // send's message to move to the highest position

int writeDriver(char* stringToSend); // send message to /dev/roboticfinger0
void sendInst(char* sizeMatrix, int posX, int posY); // to append the instruction

//////////////////////// Library Implementation ///////////////////////////////

/**
 *  @brief The move function provide the facility for the device movement through the
 *  x, y & z axes.
 *  @param posX The x axis
 *  @param pos_y The y axis
 *  @param sizeMatrix The size of the target matrix
 */
void move(int posX, int posY, int sizeMatrix)
{
    switch(sizeMatrix)    // match size of matrix 1x1, 2x2 or 4x4
    {
    case 1 :
        sendInst("1:", posX, posY); // create message to move finger to X in matrix A
        break;
    case 2 :
        sendInst("2:", posX, posY); // create message to move finger to X in matrix B
        break;
    case 3 :
        sendInst("3:", posX, posY); // create message to move finger to X in matrix C
        break;
    }
}

/**
 *  @brief The locale function provide the facility for the device movement through the
 *  x, y & z axes with the drag aditionally.
 *  @param posX The initial x axis
 *  @param posY The initial y axis
 *  @param sizeMatrix The size of the target matrix
 */
void shift(int posX, int posY, int sizeMatrix)
{
    switch(sizeMatrix)    // match size of matrix 1x1, 2x2 or 4x4
    {
    case 1 :
        sendInst("S1:", posX, posY); // create message to move finger to X in matrix A
        break;
    case 2 :
        sendInst("S2:", posX, posY); // create message to move finger to X in matrix B
        break;
    case 4 :
        sendInst("S3:", posX, posY); // create message to move finger to X in matrix C
        break;
    }
}

// @brief The press function provide's the approach to the tablet screen

void press()
{
    writeDriver("press");
}

// @brief The release function provide's the clean up for the tablet screen

void release()
{
    writeDriver("release");
}

/**
 *  @brief The init function provide's the initial position setup for the
 *  roboticFinger device
 */
void init()
{
    writeDriver("init");
}

/**
 *  @brief The high function provide's the highest position setup for the
 *  roboticFinger device
 */
void high()
{
    writeDriver("high");
}

/////////// Auxiliary functions /////////////////////

/**
 *  @brief The write_driver function provide's the real communication with the device
 *  driver in order to setup configurations and different movement's
 *  @param pStringToSend This is the string that will be send to the device.
 */
int writeDriver(char* stringToSend)
{
    Device = open("/dev/roboticFinger0", O_RDWR);  // Open the device driver with read/write access

    if (Device < 0)
    {
        perror("roboticFingerLib: Failed to open the device.");
        return -1;
    }

    Obtained = write(Device, stringToSend, strlen(stringToSend)); // Send the string to the Driver

    if (Obtained < 0)
    {
        perror("roboticFingerLib: Failed to write the message to the device.");
        return -1;
    }
}

/**
 *  @brief The send_instruction function append's the received parameter's in order to send the needed data
 *  to the roboticFinger device.
 *  @param sizeMatrix This is the matrix size (1->1x1, 2x2, 4->4x4)
 *  @param pos This is compose position that will be sended.
 */
void sendInst(char* sizeMatrix, int posX, int posY)
{
    snprintf(message, sizeof(message), "%s%d%d", sizeMatrix, posX, posY);
    writeDriver(message);
}
