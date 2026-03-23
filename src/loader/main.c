#include "playbrew.h"

static PlayBrewAPI api;

#ifdef TARGET_DVT1
#define PLAYDATE_RAM_END ((void *)0x61000000)
#endif
#ifdef TARGET_H7D1
#define PLAYDATE_RAM_END ((void *)0x91000000)
#endif

static void playbrew_loader_load(void) {
	void *payloadStart;
	unsigned int entryPoint;
	
	api.sd_open = (void *(*)(const char *, int))0x24067594;
	api.sd_close = (void (*)(void *))0x240676fc;
	api.sd_read = (void *(*)(const char *, void *, size_t))0x24067918;
	api.printf = (int (*)(const char *, ...))0x240baf50;
	
	void *fh = api.sd_open("/payload.bin", 0x83);
	
	unsigned int filesz;
	api.sd_read(fh, &filesz, sizeof(unsigned int));
	api.sd_read(fh, &entryPoint, sizeof(unsigned int));
	
	if (filesz % 4 != 0) {
		payloadStart = PLAYDATE_RAM_END - (filesz + 4 - (filesz % 4));
	}
	else {
		payloadStart = PLAYDATE_RAM_END - filesz;
	}
	
	api.sd_read(fh, (void *)payloadStart, filesz);
	
	void (*entry)(PlayBrewAPI *) = (void (*)(PlayBrewAPI *))((unsigned int)(payloadStart + entryPoint) | 1);
	api.sd_close(fh);
	
	entry(&api);
}

void playbrew_loader_start(void) __attribute__((naked));

void playbrew_loader_start(void) {
	const void *ogCall = (void *)0x240c5dbb;
	
	playbrew_loader_load();
	goto *ogCall;
}