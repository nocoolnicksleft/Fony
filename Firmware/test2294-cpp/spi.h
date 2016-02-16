#ifndef _SPI_H_
#define _SPI_H_

#define _SPI_SUPPORT_9BIT_WORKAROUND 1

#define SPI0_DIRECTION_OUT (BIT4 | BIT6 | BIT7)
#define SPI1_DIRECTION_OUT (BIT17 | BIT19 | BIT20)

#define SPI0_DIRECTION_IN (BIT5)
#define SPI1_DIRECTION_IN (BIT18)

class SPIPort
{
  protected:
    unsigned char _portid;
    volatile unsigned short int * _spcr;  // control register
    volatile unsigned char * _spsr;  // status register
    volatile unsigned short int * _spdr;  // data register
    volatile unsigned char * _spccr; // clock counter register
    volatile unsigned char * _spint; // interrupt register
    
    volatile unsigned long int * _pinsel; // interrupt register
    
    volatile unsigned long int * _direction; // gpio direction register

#ifdef _SPI_SUPPORT_9BIT_WORKAROUND
    volatile long unsigned int * _gpio_data_port;
    unsigned long int _gpio_data_bit;
    unsigned int _gpio_clock_bit;
#endif

    volatile long unsigned int * _gpio_select_port;
    unsigned int _gpio_select_bit;

  public:
    SPIPort(unsigned short int portid);

  public:
    void On();
    void Off();
    unsigned int WriteSync(unsigned int data);

#ifdef _SPI_SUPPORT_9BIT_WORKAROUND
  public:
    unsigned int WriteSync9Bit(unsigned int data);
#endif

};


#endif

