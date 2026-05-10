#include "3.0.2.h"

#include <stdint.h>

#ifdef TARGET_DVT1
#define PLAYDATE_RAM_END ((uintptr_t)0x61000000u)
#endif
#ifdef TARGET_H7D1
#define PLAYDATE_RAM_END ((uintptr_t)0x91000000u)
#endif

#define PLAYBREW_PAYLOAD_ALIGNMENT ((uintptr_t)4u)
#define PLAYBREW_PAYLOAD_ALIGNMENT_MASK (PLAYBREW_PAYLOAD_ALIGNMENT - 1u)
#define PLAYBREW_MAX_PAYLOAD_SIZE ((uintptr_t)(8u * 1024u * 1024u))
#define PLAYBREW_SAFE_LOAD_REGION_START (PLAYDATE_RAM_END - PLAYBREW_MAX_PAYLOAD_SIZE)

static int playbrew_read_exact(PlayBrewAPI *api, void *fh, void *buf, size_t len) {
	return api->sd_read(fh, buf, len) == (int)len;
}

static int playbrew_payload_start(uint32_t payloadSize, uintptr_t *payloadStartAddress) {
	if (payloadSize == 0u) {
		return 0;
	}

	uintptr_t payloadSizeAddress = (uintptr_t)payloadSize;
	if (payloadSizeAddress > UINTPTR_MAX - PLAYBREW_PAYLOAD_ALIGNMENT_MASK) {
		return 0;
	}

	uintptr_t alignedSize = (payloadSizeAddress + PLAYBREW_PAYLOAD_ALIGNMENT_MASK) & ~PLAYBREW_PAYLOAD_ALIGNMENT_MASK;
	if (alignedSize > PLAYBREW_MAX_PAYLOAD_SIZE) {
		return 0;
	}

	uintptr_t startAddress = PLAYDATE_RAM_END - alignedSize;
	if (startAddress < PLAYBREW_SAFE_LOAD_REGION_START) {
		return 0;
	}

	*payloadStartAddress = startAddress;
	return 1;
}

void playbrew_loader_load(void) {
	uint32_t payloadSize;
	uint32_t entryOffset;
	uintptr_t payloadStartAddress;
	
	PlayBrewAPI api;
	
	api.sd_open = (void *(*)(const char *, int))ADDR_SD_OPEN;
	api.sd_close = (void (*)(void *))ADDR_SD_CLOSE;
	api.sd_read = (int (*)(void *, void *, size_t))ADDR_SD_READ;
	api.printf = (void (*)(const char *, ...))ADDR_PRINTF;
	api.gfx_clear = (void (*)(int))ADDR_GFX_CLEAR;
	api.gfx_drawLine = (void (*)(int, int, int, int, int, int))ADDR_GFX_DRAWLINE;
	api.gfx_setPixel = (void (*)(int, int, int))ADDR_GFX_SETPIXEL;
	
	api.printf("started PlayBrew! ");

	void *log = api.sd_open("/PBLogs.txt", 0x88);
	if (log != NULL) {
		api.sd_close(log);
	}

	void *fh = api.sd_open("/payload.bin", 0x83);
	
	if (fh == NULL) {
		api.printf("could not open /payload.bin! Returning! ");
		return;
	}
	
	if (!playbrew_read_exact(&api, fh, &payloadSize, sizeof(payloadSize)) ||
		!playbrew_read_exact(&api, fh, &entryOffset, sizeof(entryOffset))) {
		api.printf("could not read payload header! Returning! ");
		api.sd_close(fh);
		return;
	}
	
	if (!playbrew_payload_start(payloadSize, &payloadStartAddress)) {
		api.printf("invalid payload size! Returning! ");
		api.sd_close(fh);
		return;
	}

	if (entryOffset >= payloadSize) {
		api.printf("invalid payload entry point! Returning! ");
		api.sd_close(fh);
		return;
	}
	
	if (!playbrew_read_exact(&api, fh, (void *)payloadStartAddress, payloadSize)) {
		api.printf("could not read payload body! Returning! ");
		api.sd_close(fh);
		return;
	}
	
	uintptr_t entryAddress = payloadStartAddress + (uintptr_t)entryOffset;
	void (*entry)(PlayBrewAPI *) = (void (*)(PlayBrewAPI *))(entryAddress | 1u);
	api.sd_close(fh);
	
	api.printf("running the payload! ");
	entry(&api);
	api.printf(" payload execution finished!");
}

#include "start.inc"
