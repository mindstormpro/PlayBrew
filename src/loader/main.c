#include "3.0.2.h"

// not needed rn:
//#ifdef TARGET_DVT1
//#define PLAYDATE_RAM_END ((void *)0x61000000)
//#endif
//#ifdef TARGET_H7D1
//#define PLAYDATE_RAM_END ((void *)0x91000000)
//#endif

unsigned char isAppSystem;

unsigned int playbrew_elevator(void) __attribute__((section(".firmware_hook")));


unsigned int playbrew_elevator(void) {
	PlayBrewAPI api;

	api.printf = (void (*)(const char *, ...))ADDR_PRINTF;
	api.unlockIfSystem = (unsigned int (*)(void))ADDR_UNLOCKIFSYSTEM;
	api.isSysApp = (unsigned char (*)(void))ADDR_ISSYSAPP;
	api.populateSysCAPI = (void (*)(void))ADDR_POPULATESYSCAPI;

	const char* bundle_id = *(const char**)0x240542e0;
	isAppSystem = api.isSysApp();

	if (bundle_id != NULL) {
    	api.printf("Started PB! bundle ID: %s ", bundle_id);
	} else {
	    api.printf("Started PB! current bundle ID is null :(");
	}
	

	if (isAppSystem != 0) {
		api.printf("system app? yes");
		__asm__ volatile ("svc #2");
		api.populateSysCAPI();
		return 1;
	} // up goes the elevator heheh
	api.printf("system app? no");
	return api.unlockIfSystem();
}

