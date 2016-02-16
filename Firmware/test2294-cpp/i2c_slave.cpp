

#include "i2c_slave.h"


 // #define I2CDEBUG 1

/***********************************************************/
/* CTOR                                                    */
/***********************************************************/
i2cSlave::i2cSlave(int portid, int address)
{
  _onclr = &I2CONCLR;
  _onset = &I2CONSET;
  _stat = &I2STAT;
  _dat = &I2DAT;
  _pinsel = &PINSEL0;
  _address = address;
  _status = 0;
  _timeout_ticks = 10000;
}


/***********************************************************/
/* init()                                                  */
/***********************************************************/
void i2cSlave::init()
{
  /* Initialize I2C */
  PCONP |= PCONP_PCI2C;

  PINSEL0 |= 0x50;

  I2SCLH = 0xC; /* 100 KHz */
  I2SCLL = 0xD;
  
  (* _onclr) = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC; /* clearing all flags */
  (* _onset) = I2CONSET_I2EN; /* enabling I2C */
}


/***********************************************************/
/* setStart()                                              */
/***********************************************************/
bool i2cSlave::setStart()
{
  int timeout = _timeout_ticks;

  (* _onclr) = I2CONCLR_AAC ;
  (* _onset) = I2CONSET_STA | I2CONSET_SI;
  
  (* _onclr) = I2CONCLR_SIC;
  
  
  while (!((* _onset) & I2CONSET_SI)  && (--timeout)); 

  if (!timeout) {
#ifdef I2CDEBUG
    debug_printf("I2C START Timeout %X\n",_status);
#endif
    return false;
  }
  
  _status = (* _stat);
  
#ifdef I2CDEBUG
  debug_printf("I2C START Status %X\n",_status);
#endif

  if (_status != I2C_STATUS_START) 
  { 
    //debug_printf("I2C Bus Busy, ABORTING\n");
    return false;
  }

  return true;
}

/***********************************************************/
/* setRestart()                                            */
/***********************************************************/
bool i2cSlave::setRestart()
{
  int timeout = _timeout_ticks;

  // Re-Start
  (* _onset) = I2CONSET_STA;
  (* _onclr) = I2CONCLR_SIC;

  while (!((* _onset) & I2CONSET_SI)  && (--timeout)); 
  
  if (!timeout) {
#ifdef I2CDEBUG
    debug_printf("I2C START REPEAT Timeout %X\n",_status);
#endif
    return false;
  }

  _status = (* _stat);

#ifdef I2CDEBUG
  debug_printf("I2C START REPEAT %X\n",_status);
#endif

  if (_status != I2C_STATUS_REPEATSTART) return false;

  return true;
}




/***********************************************************/
/* setSLAW()                                               */
/***********************************************************/
bool i2cSlave::setSLAW()
{
  int timeout = _timeout_ticks;

  I2DAT = (_address << 1) | 0;  // Slave Address | Write
  
  (* _onclr) = I2CONCLR_SIC;

  while (!((* _onset) & I2CONSET_SI)  && (--timeout)); 

  if (!timeout) {
#ifdef I2CDEBUG
    debug_printf("I2C SLAW Timeout %X\n",_status);
#endif
    return false;
  }

  _status = (* _stat);

#ifdef I2CDEBUG
  debug_printf("I2C SLAW Status %X\n",_status);
#endif

  if (_status != I2C_STATUS_SLAW_ACK) return false;

  return true;
}



/***********************************************************/
/* setData()                                               */
/***********************************************************/
bool i2cSlave::setData(int data)
{
  int timeout = _timeout_ticks;

  (* _onclr) = I2CONCLR_STAC;
  (* _dat) = data; // Pointer Byte 
  (* _onclr) = I2CONCLR_SIC;

  while (!((* _onset) & I2CONSET_SI)  && (--timeout)); 

  if (!timeout) {
#ifdef I2CDEBUG
    debug_printf("I2C TRANSMIT Timeout %X\n",_status);
#endif
    return false;
  }

  _status = (* _stat);

#ifdef I2CDEBUG
  debug_printf("I2C TRANSMIT Status %X\n",_status);
#endif

  if (_status != I2C_STATUS_TRANSMIT_ACK) return false;

  return true;
} 



/***********************************************************/
/* setSLAR()                                               */
/***********************************************************/
bool i2cSlave::setSLAR()
{
  int timeout = _timeout_ticks;

  (* _onclr) = I2CONCLR_STAC;
  (* _dat) = (_address << 1) | 1;  // Slave Address | Read
  (* _onclr) = I2CONCLR_SIC;

  while (!((* _onset) & I2CONSET_SI)  && (--timeout)); 
  
  if (!timeout) {
#ifdef I2CDEBUG
    debug_printf("I2C SLAR Timeout %X\n",_status);
#endif
    return false;
  }

  _status = (* _stat);

#ifdef I2CDEBUG
  debug_printf("I2C SLAR Status %X\n",_status);
#endif

  if (_status != I2C_STATUS_SLAR_ACK) return false;

  return true;
}
 


/***********************************************************/
/* readByte()                                              */
/***********************************************************/
unsigned char i2cSlave::readByte()
{
  int timeout = _timeout_ticks;

  (* _onset) = I2CONSET_AA;
  (* _onclr) = I2CONCLR_SIC;

  while (!((* _onset) & I2CONSET_SI)  && (--timeout)); 
  
  if (!timeout) {
#ifdef I2CDEBUG
    debug_printf("I2C READ Timeout %X\n",_status);
#endif
    return false;
  }

  _status = (* _stat);

#ifdef I2CDEBUG
  debug_printf("I2C READ Status %X\n",_status);
#endif

  if (_status != I2C_STATUS_RECEIVE_ACK) return 0;

  return (* _dat);
}



/***********************************************************/
/* readLastByte()                                          */
/***********************************************************/
unsigned char i2cSlave::readLastByte()
{
  int timeout = _timeout_ticks;

  (* _onclr) = I2CONCLR_STAC;
  (* _onclr) = I2CONCLR_AAC;
  (* _onclr) = I2CONCLR_SIC;

  while (!((* _onset) & I2CONSET_SI)  && (--timeout)); 

  if (!timeout) {
#ifdef I2CDEBUG
    debug_printf("I2C READLAST Timeout %X\n",_status);
#endif
    return false;
  }

  _status = (* _stat);

#ifdef I2CDEBUG
  debug_printf("I2C READLAST Status %X\n",_status);
#endif

  if (_status != I2C_STATUS_RECEIVE_NACK) return 0;

  return (* _dat);
}




/***********************************************************/
/* setStop()                                               */
/***********************************************************/
void i2cSlave::setStop()
{
  (* _onset) = I2CONSET_STO;
  (* _onclr) = I2CONCLR_SIC;

  _status = (* _stat);

  (* _onclr) = I2CONCLR_I2ENC; /* switch off */

#ifdef I2CDEBUG
  debug_printf("I2C STOP Status %X\n",_status);
#endif
}

int i2cSlave::status()
{
  return _status;
}



