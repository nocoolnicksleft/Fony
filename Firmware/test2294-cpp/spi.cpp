

#include "common.h"

#include "spi.h"

SPIPort::SPIPort(unsigned short int portid)
{
  _portid = portid;
  if (portid == 0) // SPI PORT 0
  {
    _spcr  = &S0SPCR;  // control register
    _spsr  = &S0SPSR;  // status register
    _spdr  = &S0SPDR;  // data register
    _spccr = &S0SPCCR; // clock counter register
    _spint = &S0SPINT; // interrupt register
#ifdef _SPI_SUPPORT_9BIT_WORKAROUND
    _gpio_data_port = &IO0PIN;
    _gpio_data_bit  = (1 << 6);
    _gpio_clock_bit = (1 << 4);
#endif
    _pinsel = &PINSEL0;
    _direction = &IO0DIR;
  
  } else {         // SPI PORT 1
    _spcr  = &S1SPCR;  // control register
    _spsr  = &S1SPSR;  // status register
    _spdr  = &S1SPDR;  // data register
    _spccr = &S1SPCCR; // clock counter register
    _spint = &S1SPINT; // interrupt register
#ifdef _SPI_SUPPORT_9BIT_WORKAROUND
    _gpio_data_port = &IO0PIN;
    _gpio_data_bit  = (1 << 19);
    _gpio_clock_bit = (1 << 17);
#endif
    _pinsel = &PINSEL1;
    _direction = &IO0DIR;
  }
}

#define PINSEL_ON (0xAA << 2)
#define PINSEL_OFF (0xFF << 2)
#define LCD_SPI_CCR S1SPCCR
#define LCD_CR S1SPCR
#define SPI_DIVIDER_VALUE 0x000000006

#ifdef _SPI_SUPPORT_9BIT_WORKAROUND
unsigned int SPIPort::WriteSync9Bit(unsigned int data)
{
    while(((* _spsr) & S0SPSR_SPIF) == 0);                  // if still busy wait here

    (* _pinsel) &= ~PINSEL_OFF; // SPI AUS

    if (data & (1 << 8))
      (* _gpio_data_port) |= _gpio_data_bit;
    else
      (* _gpio_data_port) &= ~_gpio_data_bit;

    (* _gpio_data_port) |= _gpio_clock_bit;
    (* _gpio_data_port) &= ~_gpio_clock_bit;

    (* _pinsel) |= PINSEL_ON; // SPI EIN

    (* _spccr) = SPI_DIVIDER_VALUE; // Clock Divider, FOR LPC2294: min 0x8, FOR LPC2148: min 0x0, even only!!
    (* _spcr) = (1 << 5); // Master mode

    (* _spdr) = data;                                         // Send the data using 9 bits


    return (* _spdr);
}
#endif

void SPIPort::On()
{
  if (_portid == 0)
  {
    PCONP |= PCONP_PCSPI0;
    (* _direction) |= SPI0_DIRECTION_OUT;
    (* _direction) &= ~SPI0_DIRECTION_IN;
  } else {
    PCONP |= PCONP_PCSPI1;
    (* _direction) |= SPI1_DIRECTION_OUT;
    (* _direction) &= ~SPI1_DIRECTION_IN;
  }
  (* _pinsel) &= ~PINSEL_OFF; // SPI AUS
  (* _pinsel) |= PINSEL_ON;   // SPI EIN
  (* _spccr)   = SPI_DIVIDER_VALUE; // Clock Divider, 4 min, even only!!
  (* _spcr)    = (1 << 5); // Master mode
  (* _spdr)    = 0;
}

void SPIPort::Off()
{
  (* _pinsel) &= ~PINSEL_OFF; // SPI AUS
  if (_portid == 0)
  {
    PCONP &= ~PCONP_PCSPI0;
  } else {
    PCONP &= ~PCONP_PCSPI1;
  }
}

unsigned int SPIPort::WriteSync(unsigned int data)
{
    (* _spdr) = data;                                         // Send the data using 9 bits

    while(((* _spsr) & S0SPSR_SPIF) == 0);                  // if still busy wait here

    return (* _spdr);
}

