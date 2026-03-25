#include "playbrew.h"

#ifdef TARGET_DVT1
#define PLAYDATE_RAM_END ((void *)0x61000000)
#endif
#ifdef TARGET_H7D1
#define PLAYDATE_RAM_END ((void *)0x91000000)
#endif

void playbrew_loader_load(void) {
	void *payloadStart;
	unsigned int entryPoint;
	
	PlayBrewAPI api;
	
	api.sd_open = (void *(*)(const char *, int))0x24067594;
	api.sd_close = (void (*)(void *))0x240676fc;
	api.sd_read = (int (*)(void *, void *, size_t))0x24067918;
	api.printf = (void (*)(const char *, ...))0x240baf50;
	api.gfx_clear = (void (*)(int))0x24055d5c;
	api.gfx_drawLine = (void (*)(int, int, int, int, int, int))0x24055be5;
	api.gfx_setPixel = (void (*)(int, int, int))0x24055901;
	
	api.printf("started PlayBrew!");
	
	void *fh = api.sd_open("/payload.bin", 0x83);
	
	if (fh == NULL) {
		api.printf("could not open /payload.bin!");
		return;
	}
	
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
	
	api.printf("running the payload!");
	entry(&api);
	api.printf("payload execution finished!");
}

#include "start.inc"