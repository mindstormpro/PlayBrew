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


	api.ucfs_open = (void *(*)(char *, int))ADDR_UCFS_OPEN;
	api.ucfs_close = (void (*)(void *))ADDR_UCFS_CLOSE;
	api.ucfs_read = (int (*)(void *, void *, size_t))ADDR_UCFS_READ;
	api.printf = (void (*)(const char *, ...))ADDR_PRINTF;
	api.strcpy = (char *(*)(char *, const char *))ADDR_STRCPY;
	api.gfx_clear = (void (*)(int))ADDR_GFX_CLEAR;
	api.gfx_drawLine = (void (*)(int, int, int, int, int, int))ADDR_GFX_DRAWLINE;
	api.gfx_setPixel = (void (*)(int, int, int))ADDR_GFX_SETPIXEL;
	api.malloc = (void *(*)(size_t))ADDR_MALLOC;

	api.printf("  started PlayBrew!  ");

	//by moving the path into memory instead of reading off of the firmware it might not crash with the string reading error... 
	//time to find out

	char *payloadPath = "/playbrew/payload.bin";
	char *pathInMem = api.malloc(22); //22 is strlen(payloadPath) + 1
	api.strcpy(pathInMem, payloadPath);
	void *fh = api.ucfs_open(pathInMem, (unsigned int)0x83);


	if (fh == NULL) {
		api.printf("   could not open /playbrew/payload.bin! Returning!   ");
		return;
	}

	unsigned int filesz;
	api.ucfs_read(fh, &filesz, sizeof(unsigned int));
	api.ucfs_read(fh, &entryPoint, sizeof(unsigned int));
	if (filesz % 4 != 0) {
		payloadStart = PLAYDATE_RAM_END - (filesz + 4 - (filesz % 4));
	}
	else {
		payloadStart = PLAYDATE_RAM_END - filesz;
	}
	
	api.ucfs_read(fh, (void *)payloadStart, filesz);
	api.ucfs_close(fh);
	api.printf("  Finished all reading and closed the payload!  ");

	api.printf("  running the payload!  ");

	void (*entry)(PlayBrewAPI *) = (void (*)(PlayBrewAPI *))((unsigned int)(payloadStart + entryPoint) | 1);
	entry(&api);

	api.printf("  payload execution finished!  ");
	
	
	
}

#include "start.inc"