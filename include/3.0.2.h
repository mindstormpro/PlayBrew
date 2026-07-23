#ifndef PLAYBREW_H
#define PLAYBREW_H

#include <stddef.h>

typedef struct {
	void (*printf)(const char *format, ...);
	unsigned int (*unlockIfSystem)(void);
	unsigned char (*isSysApp)(void);
	
} PlayBrewAPI;


#define ADDR_PRINTF ((void (*)(const char *, ...))0x240baf51)
#define ADDR_UNLOCKIFSYSTEM ((void (*)(void))0x24051741)
#define ADDR_ISSYSAPP ((unsigned char (*)(void))0x24041435)



#endif