#include "3.0.2.h"


#ifdef TARGET_DVT1
#define PLAYDATE_RAM_END ((void *)0x61000000)
#endif
#ifdef TARGET_H7D1
#define PLAYDATE_RAM_END ((void *)0x91000000)
#endif

void playbrew_elevator(void) {
	
}

#include "start.inc"