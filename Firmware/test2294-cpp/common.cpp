
#include "common.h"


u16 * LoadFile(u16 id)
{
	u16 * fileptr = 0;
        if (id == 1) return (u16 *)bigpinky_kitzel_0_0;
	return fileptr;
}

u16 * LoadFileCopy(u16 id)
{
	u8 * fileptr = 0;
	u8 * newfileptr = 0;
	u32 size = 0;
        if (id == 1)
        { 
          fileptr = (u8 *)bigpinky_kitzel_0_0;
          size = bigpinky_kitzel_0_0_size;
        }
	
	if (size)
	{
		newfileptr = new uint8[size];
		memcpy(newfileptr,fileptr,size);
		//sprintf(filename,"%u.bin size %u ptr %u",id,size,fileptr);
		//DebugPrint(filename);
	} 
	return (u16 *)newfileptr;

}


u32 FileSize(u16 id)
{
	u32 size;
	return size;
}


