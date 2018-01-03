/*
 * @file    pig2i2c.c
 * 
 * @brief   I2C Sniffer for Raspberry Pi using pigpio library
 * 
 * As seen in http://abyz.me.uk/rpi/pigpio/examples.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

#include "pigpio.h"

/*
This software reads pigpio notification reports monitoring the I2C signals.

Notifications are pipe based so this software must be run on the Pi
being monitored.

It should be able to handle a 100kHz bus.  You are unlikely to get any
usable results if the bus is running at 400kHz.

gcc -o pig2i2c pig2i2c.c

Do something like

sudo pigpiod -s 2

# get a notification handle, assume handle 0 was returned

pigs no

# start notifications for SCL/SDA

e.g. pigs nb 0 0x3   # Rev. 1 select gpios 0/1
e.g. pigs nb 0 0xC   # Rev. 2 select gpios 2/3
e.g. pigs nb 0 0xA00 # select gpios 9/11 (1<<9|1<<11)

# run the program, specifying SCL/SDA and notification pipe

./pig2i2c SCL SDA </dev/pigpioN # specify gpios for SCL/SDA and pipe N

e.g. ./pig2i2c 1  0 </dev/pigpio0 # Rev.1 I2C gpios
e.g. ./pig2i2c 3  2 </dev/pigpio0 # Rev.2 I2C gpios
e.g. ./pig2i2c 9 11 </dev/pigpio0 # monitor external bus 
*/

/** gpio Report RS */
#define RS (sizeof(gpioReport_t))

/** Clock falling */
#define SCL_FALLING 0
/** Clock rising */
#define SCL_RISING  1
/** Clock steady */
#define SCL_STEADY  2

/** Data falling */
#define SDA_FALLING 0
/** Data rising */
#define SDA_RISING  4
/** Data steady */
#define SDA_STEADY  8

/** Standby */
#define STANDBY     0
/** Packet read */
#define READ        1

/** Maximum packet size */
#define MAX_SIZE    32

/**
 * @brief      Gets the current timestamp.
 *
 * @return     The buffer holding the timestamp string.
 */
static char * timeStamp()
{
    static char buf[32];

    struct timeval now;
    struct tm tmp;

    gettimeofday(&now, NULL);

    localtime_r(&now.tv_sec, &tmp);
    strftime(buf, sizeof(buf), "%F %T", &tmp);

    return buf;
}

/**
 * @brief Converts float to byte array and vice-versa.
 *
 * As seen in <a href="http://mbed.org/forum/helloworld/topic/2053/">link</a>
 */
typedef union float_to_bytes_t{
    /** Float variable */
    float f;
    /** Float byte array */
    byte b[sizeof(float)];
} float_bytes;

/**
 * @brief      Writes a packet to a file.
 *
 * @param[in]  fd      The file descriptor.
 * @param      packet  The packet
 * @param[in]  size    The size
 */
void writePacket(int fd, uint8_t *packet, size_t size)
{
    if (size >=  2){
        write(fd, packet, size);
    }
}

/**
 * @brief      Parses I2C, one byte at a time
 *
 * @param[in]  SCL    The scl (clock)
 * @param[in]  SDA    The sda (data)
 * @param      valid  Whether the byte is valid
 * @param      start  Whether it is the end of a packet
 * @param      end    Whether it is the end of a packet
 * @param      acked  Whether byte was acked
 *
 * @return     Reads a byte
 */
uint8_t parse_I2C( int SCL, int SDA, bool & valid,
               bool & start, bool & end, bool & acked)
{
    static int in_data=0, byte=0, bit=0;
    static int oldSCL=1, oldSDA=1;

    int xSCL, xSDA;

    // Data byte read
    uint8_t rv = 0;

    if (SCL != oldSCL)
    {
        oldSCL = SCL;
        if (SCL) {
            xSCL = SCL_RISING;
        } else {   
            xSCL = SCL_FALLING;
        }
    }
    else
    {
        xSCL = SCL_STEADY;
    }

    if (SDA != oldSDA)
    {
        oldSDA = SDA;
        if (SDA) {
            xSDA = SDA_RISING;
        } else {
            xSDA = SDA_FALLING;
        }
    }
    else
    {
        xSDA = SDA_STEADY;
    }

    switch (xSCL+xSDA)
    {
        case SCL_RISING + SDA_RISING:
        case SCL_RISING + SDA_FALLING:
        case SCL_RISING + SDA_STEADY:
            if (in_data)
            {
                if (bit++ < 8)
                {
                    byte <<= 1;
                    byte |= SDA;
                }
                else
                {
                    //printf("%02X", byte);
                    rv = byte;
                    valid = true;

                    if (SDA){
                        //printf("-");
                        acked = false;
                    } else {
                        //printf("+");
                        acked = true;
                    }
                    bit = 0;
                    byte = 0;
                }
            }
            break;

        case SCL_FALLING + SDA_RISING:
            break;

        case SCL_FALLING + SDA_FALLING:
            break;

        case SCL_FALLING + SDA_STEADY:
            break;

        case SCL_STEADY + SDA_RISING:
            if (SCL)
            {
                in_data = 0;
                byte = 0;
                bit = 0;

                //printf("]\n"); // stop
                end = true;
                fflush(NULL);
            }
            break;

        case SCL_STEADY + SDA_FALLING:
            if (SCL)
            {
                in_data = 1;
                byte = 0;
                bit = 0;

                //printf("["); // start
                start = true;
            }
            break;

        case SCL_STEADY + SDA_STEADY:
            break;

    }
    return rv;
}

/**
 * @brief      Executes the main loop.
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 *
 * @return     0 on success, -1 otherwise
 */
int main(int argc, char * argv[])
{
    int gSCL, gSDA, SCL, SDA, xSCL;
    int r;
    uint32_t level, changed, bI2C, bSCL, bSDA;

    gpioReport_t report;

    if (argc > 3)
    {
        gSCL = atoi(argv[1]);
        gSDA = atoi(argv[2]);

        bSCL = 1<<gSCL;
        bSDA = 1<<gSDA;

        bI2C = bSCL | bSDA;
    }
    else
    {
        printf("Usage: ./pig2i2c.bin <SCL> <SDA> <path/to/FIFO>\n");
        exit(-1);
    }

    /* default to SCL/SDA high */

    SCL = 1;
    SDA = 1;
    level = bI2C;
    
    // Variables for packet creation 
    bool valid, start, end, acked;
    uint8_t recv             = 0;
    size_t  idx              = 0;
    size_t  size             = 0;
    uint8_t packet[MAX_SIZE] = {0};
    // Current program state
    int state                = STANDBY;

    // Output FIFO
    int fifo_fd; 
    char fifo_path[100];    
    strncpy(fifo_path, argv[3], 100);
    // Check if FIFO exists. If not, create it
    struct stat st;
    if (stat(fifo_path, &st) != 0){
        printf("[INFO] FIFO not found. Creating %s\n", fifo_path);
        mkfifo(fifo_path, 0666);
    }
    // Open FIFO for writing
    fifo_fd = open(fifo_path, O_WRONLY);
    if (fifo_fd < 0){
        fprintf(stderr, "[ERROR] Could not open FIFO\n");
        exit(-1);
    }

    // Sniff I2C communications and redirect packets to FIFO
    while ((r=read(STDIN_FILENO, &report, RS)) == RS)
    {
        report.level &= bI2C;

        if (report.level != level)
        {
            changed = report.level ^ level;

            level = report.level;

            if (level & bSCL) SCL = 1; else SCL = 0;
            if (level & bSDA) SDA = 1; else SDA = 0;
            
            valid = start = end = acked = false;
            recv = parse_I2C(SCL, SDA, valid, start, end, acked);

            if (state == STANDBY && start){
                state = READ;
                idx = 0;
            }
            else if (state == READ) {
                if (end) {
                    state = STANDBY;
                    size = idx;
                    
                    // First byte of packet can be discarded
                    writePacket(fifo_fd, packet + 1, size - 1);

                    idx = 0;
                } else if (valid) {
                   packet[idx++] = recv;
                }
            }
        }
    }
    unlink(fifo_path);
    return 0;
}

