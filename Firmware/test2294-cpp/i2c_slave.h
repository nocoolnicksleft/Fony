

#ifndef _I2C_SLAVE_H_
#define _I2C_SLAVE_H_

#include <libarm.h>

#include "common.h"
#include "ringbuffer.h"

// I2C STATUS VALUES FOR MASTER / MASTER-RECEIVER MODES
#define I2C_STATUS_START 0x08
#define I2C_STATUS_REPEATSTART 0x10
#define I2C_STATUS_SLAW_ACK 0x18
#define I2C_STATUS_SLAW_NACK 0x20
#define I2C_STATUS_TRANSMIT_ACK 0x28
#define I2C_STATUS_TRANSMIT_NACK 0x30
#define I2C_STATUS_ARBITRATIONLOST_WRITE 0x38
#define I2C_STATUS_SLAR_ACK 0x40
#define I2C_STATUS_SLAR_NACK 0x48
#define I2C_STATUS_RECEIVE_ACK 0x50
#define I2C_STATUS_RECEIVE_NACK 0x58

typedef unsigned int            uint32;
typedef volatile unsigned long  vuint32;

class i2cSlave
{

  protected:
    uint32 _address;
  
    volatile unsigned char * _onclr;
    volatile unsigned char * _onset;
    volatile unsigned char * _stat;
    volatile unsigned char * _dat;

    volatile unsigned long int * _pinsel; // function selection register
    
    int _timeout_ticks;
  
    int _status;

  public:
    i2cSlave(int portid, int address);

    void init();

    bool setStart();
    bool setRestart();
    bool setSLAW();
    bool setData(int data);
    bool setSLAR();
    unsigned char readByte();
    unsigned char readLastByte();
    void setStop();

    int status();

};


#endif

