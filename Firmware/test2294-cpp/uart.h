
#ifndef _UART_H_
#define _UART_H_

#define SUPPORT_UART0 1 
#define SUPPORT_UART1 1

extern "C" static void UART0_RX_ISR(void) __attribute__ ((interrupt ("IRQ")));
extern "C" static void UART0_TX_ISR(void) __attribute__ ((interrupt ("IRQ")));


class UARTHandler
{
  char * rxBuffer;
  char * txBuffer;

public:
  UARTHandler();
  ~UARTHandler();

public:
  void Init(unsigned int baud);
  static void HookInterrupts();


  friend void UART0_RX_ISR(void);
  friend void UART0_TX_ISR(void);
        
};

#endif

