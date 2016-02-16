

/* I2C ISR */

__irq void I2C _ISR(void);

/* Master Transmitter states */
void ISR_8(void);
void ISR_18(void);
void ISR_28(void);

/*************************** MAIN ************************/
int main()
{
  /* Initialize system */
  Initialize ();
  /* Send start bit */
  I2CONSET=0x60;
  /* Do forever */
  while(1)
  {
    IOCLR=0x40;
    IOSET=0x40;
  }
}

/*************** System Initialization ***************/
void Initialize()
{
  /* Remap interrupt vectors to SRAM */
  MEMMAP = 0x2;
  /* Initialize GPIO ports to be used as indicators */
  IODIR = 0xF0;
  IOSET = 0xF0;
  /* Initialize Pin Connect Block */
  PINSEL0 = 0x50;
  /* Initialize I2C */
  I2CONCLR = 0x6c; /* clearing all flags */
  I2CONSET = 0x40; /* enabling I2C */
  I2SCLH = 0xC; /* 100 KHz */
  I2SCLL = 0xD;
  /* Initialize VIC for I2C use */
  VICINTSEL = 0x0; /* selecting IRQ */
  VICINTEN = 0x200; /* enabling I2C */
  VICCNTL0 = 0x29; /* highest priority and enabled */
  VICVADDR0 = (unsigned long) I2C_ISR;
  /* ISR address written to the respective address register*/
}

/********************** I2C ISR **************************/
__irq void I2C_ISR()
{
  int temp=0;
  temp=I2STAT;
  switch(temp)
  {

    
    case 0x08: // START TRANSMITTED
      ISR_8();
      
      break;

    case 0x10: // A repeated START condition has been transmitted
      break;
    case 0x18: // SLA+W has been transmitted; ACK has been received
      ISR_18();
      break;
    case 0x20: // SLA+W has been transmitted; NOT ACK has been received
      break;
    case 0x28: // Data byte in S1DAT has been transmitted; ACK has been received
      ISR_28();
      break;
    case 0x30: // Data byte in S1DAT has been transmitted; NOT ACK has been received
      break;
    case 0x38: // Arbitration lost in SLA+R/W or Data bytes
      break;
    case 0x40: // SLA+R has been transmitted; ACK has been received
      break;
    case 0x48: // SLA+R has been transmitted; NOT ACK has been received
      break;
    case 0x50: // Data byte has been received; ACK has been returned
      break;
    case 0x58: // Data byte has been received; NOT ACK has been returned
      break;
    default :
      break;
  }
  VICVADDR=0xFF;
}

/* I2C states*/
/* Start condition transmitted */
void ISR_8()
{
  /* Port Indicator */
  IOCLR=0x10;
  /* Slave address + write */
  I2DAT=0x74;
  /* Clear SI and Start flag */
  I2CONCLR=0x28;
  /* Port Indicator */
  IOSET=0x10;
}

/* Acknowledgement received from slave for slave address */
void ISR_18()
{
  /* Port Indicator */
  IOCLR=0x20;
  /* Data to be transmitted */
  I2DAT=0x55;
  /* clear SI */
  I2CONCLR=0x8;
  /* Port Indicator */
  IOSET=0x20;
}

/* Acknowledgement received from slave for byte transmitted from master. Stop
condition is transmitted in this state signaling the end of transmission */
void ISR_28()
{
  /* Port Indicator */
  IOCLR=0x80;
  /* Transmit stop condition */
  I2CONSET=0x10;
  /* clear SI */
  I2CONCLR=0x8;
  /* Port Indicator */
  IOSET=0x80;
}

