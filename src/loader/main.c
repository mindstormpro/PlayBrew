#include "3.0.2.h"

// not needed rn:
//#ifdef TARGET_DVT1
//#define PLAYDATE_RAM_END ((void *)0x61000000)
//#endif
//#ifdef TARGET_H7D1
//#define PLAYDATE_RAM_END ((void *)0x91000000)
//#endif

unsigned char isAppSystem;

void playbrew_elevator(void) {
	PlayBrewAPI api;

	api.printf = (void (*)(const char *, ...))ADDR_PRINTF;
	api.unlockIfSystem = (unsigned int (*)(void))ADDR_UNLOCKIFSYSTEM;
	api.isSysApp = (unsigned char (*)(void))ADDR_ISSYSAPP;

	api.unlockIfSystem();
	isAppSystem = api.isSysApp();
	if (isAppSystem != 0) {__asm__ volatile ("svc #2");} // up goes the elevator heheh
	

}

#include "start.inc"