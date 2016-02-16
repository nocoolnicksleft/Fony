
#include <targets/LPC2000.h>
#include <targets/liblpc2000.h>

#include <targets/lpc2294.h>
#include <ctl_api.h>
#include <libarm.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef signed char int8;
typedef unsigned char uint8;
typedef unsigned char u8;

typedef short signed int int16;
typedef short unsigned int uint16;
typedef short unsigned int u16;

typedef signed int int32;
typedef unsigned int uint32;
typedef unsigned int u32;

#define BIT0 0x00000001
#define BIT1 0x00000002
#define BIT2 0x00000004
#define BIT3 0x00000008
#define BIT4 0x00000010
#define BIT5 0x00000020
#define BIT6 0x00000040
#define BIT7 0x00000080
#define BIT8 0x00000100
#define BIT9 0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000


#define RGB_B(n) ((n & 0x7C00) >> 10 )
#define RGB_G(n) ((n & 0x03E0) >>  5 )
#define RGB_R(n) ((n & 0x001F)       )

#define RGB_B_NS(n) ((n & 0x7C00))
#define RGB_G_NS(n) ((n & 0x03E0))
#define RGB_R_NS(n) ((n & 0x001F))


extern "C" {
  #include <efs.h>
  #include <ls.h>
  #include "debug.h"

  #include "bigpinky_kitzel_0_0.h"


  u16 * LoadFile(u16 id);
  u16 * LoadFileCopy(u16 id);
  u32 FileSize(u16 id);

}



