#include "3.0.2.h"


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
	
	api.sd_open = (void *(*)(const char *, int))ADDR_SD_OPEN;
	api.sd_close = (void (*)(void *))ADDR_SD_CLOSE;
	api.sd_read = (int (*)(void *, void *, size_t))ADDR_SD_READ;
	api.printf = (void (*)(const char *, ...))ADDR_PRINTF;
	api.gfx_clear = (void (*)(int))ADDR_GFX_CLEAR;
	api.gfx_drawLine = (void (*)(int, int, int, int, int, int))ADDR_GFX_DRAWLINE;
	api.gfx_setPixel = (void (*)(int, int, int))ADDR_GFX_SETPIXEL;
	
	api.printf("started PlayBrew!\n");
	
	
	void *fh = api.sd_open("/payload.bin", 0x83);
	
	if (fh == NULL) {
		api.printf("could not open /payload.bin!\nReturning!\n");
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
	
	api.printf("running the payload!\n");
	entry(&api);
	api.printf("payload execution finished!\n");
}

#include "start.inc"