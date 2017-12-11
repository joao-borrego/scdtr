/*
 * @file pig2i2c.c
 * 
 * @brief I2C Sniffer for Raspberry Pi using pigpio library
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

#define RS (sizeof(gpioReport_t))

#define SCL_FALLING 0
#define SCL_RISING  1
#define SCL_STEADY  2

#define SDA_FALLING 0
#define SDA_RISING  4
#define SDA_STEADY  8

#define STANDBY 0
#define READ    1

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

int parse_I2C(int SCL, int SDA, bool & valid,
                bool & start, bool & end, bool & acked)
{
    static int in_data=0, byte=0, bit=0;
    static int oldSCL=1, oldSDA=1;

    int xSCL, xSDA;

    int rv = 0;

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

typedef union float_bytes_t{
    float f;
    unsigned char b[sizeof(float)];
} float_bytes;

void decodePacket(int *buffer, size_t size)
{
    // Note: first byte in i2c buffer is useless to us
    if (size < 3){
        // Empty packet
        return;
    }
    
    int id = buffer[1];
    int type = buffer[2];
    printf("Id %d Type %d - ", id, type);
   
    if (type == 2 && size == 11){
        for (int i = 0; i < 2; i++){
            float_bytes fb;
            for (int j = 0; j < sizeof(float); j++){
                fb.b[j] = buffer[1 + 2 + i * sizeof(float) + j];
            }
            printf("%.3f ", fb.f);
        }
    }

    printf("\n");
    fflush(NULL);                

}

int main(int argc, char * argv[])
{
    int gSCL, gSDA, SCL, SDA, xSCL;
    int r;
    uint32_t level, changed, bI2C, bSCL, bSDA;

    gpioReport_t report;

    if (argc > 2)
    {
        gSCL = atoi(argv[1]);
        gSDA = atoi(argv[2]);

        bSCL = 1<<gSCL;
        bSDA = 1<<gSDA;

        bI2C = bSCL | bSDA;
    }
    else
    {
        exit(-1);
    }

    /* default to SCL/SDA high */

    SCL = 1;
    SDA = 1;
    level = bI2C;

    bool valid, start, end, acked;
    unsigned char recv = 0;
    
    size_t idx = 0;
    size_t size = 0;
    int  packet[32] = {0};

    int state = 0;

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
                    
                    decodePacket(packet, size);

                    idx = 0;
                } else if (valid) {
                   packet[idx++] = recv;
                }
            }
        }
    }
    return 0;
}

