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
	void *temp = ADDR_UCFS_CLOSE;

	api.ucfs_open = (void *(*)(const char *, int))ADDR_UCFS_OPEN;
	api.ucfs_close = (void (*)(void *))*temp;
	api.ucfs_read = (int (*)(void *, void *, size_t))ADDR_UCFS_READ;
	api.printf = (void (*)(const char *, ...))ADDR_PRINTF;
	api.gfx_clear = (void (*)(int))ADDR_GFX_CLEAR;
	api.gfx_drawLine = (void (*)(int, int, int, int, int, int))ADDR_GFX_DRAWLINE;
	api.gfx_setPixel = (void (*)(int, int, int))ADDR_GFX_SETPIXEL;
	api.printf("  started PlayBrew!  ");
	void *fh = api.fopen("/playbrew/payload.bin", "rb");

	if (fh == NULL) {
		api.printf("   could not open /playbrew/payload.bin! Returning!   ");
		return;
	}

	unsigned int filesz;
	api.fread(&filesz, sizeof(unsigned int), 1, fh);
	api.fread(&entryPoint, sizeof(unsigned int), 1, fh);
	if (filesz % 4 != 0) {
		payloadStart = PLAYDATE_RAM_END - (filesz + 4 - (filesz % 4));
	}
	else {
		payloadStart = PLAYDATE_RAM_END - filesz;
	}
	
	api.fread((void *)payloadStart, filesz, 1, fh);
	api.fclose(fh);
	api.printf("  Finished all reading and closed the payload!  ");

	api.printf("  running the payload!  ");

	void (*entry)(PlayBrewAPI *) = (void (*)(PlayBrewAPI *))((unsigned int)(payloadStart + entryPoint) | 1);
	entry(&api);

	api.printf("  payload execution finished!  ");
	
	
	
}

#include "start.inc"