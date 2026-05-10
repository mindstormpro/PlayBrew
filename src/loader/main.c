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
#define PLAYBREW_PAYLOAD_PATH "/payload.bin"
#define PLAYBREW_LOG_SMOKE_TEST_PATH "/PBLogs.txt"
#define PLAYBREW_SD_MODE_READ 0x83
#define PLAYBREW_SD_MODE_WRITE 0x88

typedef struct {
	size_t totalRead;
	int lastRead;
} PlayBrewReadStatus;

static int playbrew_read_exact(PlayBrewAPI *api, void *fh, void *buf, size_t len, PlayBrewReadStatus *status) {
	uint8_t *out = (uint8_t *)buf;

	status->totalRead = 0u;
	status->lastRead = 0;

	while (status->totalRead < len) {
		size_t remaining = len - status->totalRead;
		status->lastRead = api->sd_read(fh, out + status->totalRead, remaining);

		if (status->lastRead <= 0) {
			return 0;
		}

		if ((size_t)status->lastRead > remaining) {
			return 0;
		}

		status->totalRead += (size_t)status->lastRead;
	}

	return 1;
}

static void playbrew_log_read_failure(PlayBrewAPI *api, const char *section, const PlayBrewReadStatus *status, size_t expected) {
	api->printf("payload: %s read failed (got %u of %u bytes, last sd_read=%d). ",
		section,
		(unsigned int)status->totalRead,
		(unsigned int)expected,
		status->lastRead);
}

static uint32_t playbrew_read_le32(const uint8_t bytes[4]) {
	return ((uint32_t)bytes[0]) |
		((uint32_t)bytes[1] << 8) |
		((uint32_t)bytes[2] << 16) |
		((uint32_t)bytes[3] << 24);
}

static int playbrew_entry_offset_is_valid(uint32_t payloadSize, uint32_t entryOffset) {
	if (entryOffset >= payloadSize) {
		return 0;
	}

	if ((entryOffset & 1u) != 0u) {
		return 0;
	}

	if (payloadSize - entryOffset < 2u) {
		return 0;
	}

	return 1;
}

static void playbrew_log_entry_failure(PlayBrewAPI *api, uint32_t payloadSize, uint32_t entryOffset) {
	if (entryOffset >= payloadSize) {
		api->printf("payload: entry offset %u is outside payload size %u. ", entryOffset, payloadSize);
		return;
	}

	if ((entryOffset & 1u) != 0u) {
		api->printf("payload: entry offset %u is not Thumb-aligned. ", entryOffset);
		return;
	}

	api->printf("payload: entry offset %u leaves less than one instruction in %u byte payload. ",
		entryOffset,
		payloadSize);
}

static void playbrew_log_load_details(PlayBrewAPI *api, uintptr_t payloadStartAddress, uint32_t payloadSize, uint32_t entryOffset) {
	uintptr_t entryAddress = payloadStartAddress + (uintptr_t)entryOffset;
	api->printf("payload: loading %u bytes at 0x%08x, entry=0x%08x. ",
		payloadSize,
		(unsigned int)payloadStartAddress,
		(unsigned int)entryAddress);
}

static void playbrew_log_header(PlayBrewAPI *api, uint32_t payloadSize, uint32_t entryOffset) {
	api->printf("payload: header size=%u entryOffset=%u. ",
		payloadSize,
		entryOffset);
}

static int playbrew_read_payload_header(PlayBrewAPI *api, void *fh, uint32_t *payloadSize, uint32_t *entryOffset) {
	uint8_t payloadSizeBytes[4];
	uint8_t entryOffsetBytes[4];
	PlayBrewReadStatus status;

	if (!playbrew_read_exact(api, fh, payloadSizeBytes, sizeof(payloadSizeBytes), &status)) {
		playbrew_log_read_failure(api, "size header", &status, sizeof(payloadSizeBytes));
		return 0;
	}

	if (!playbrew_read_exact(api, fh, entryOffsetBytes, sizeof(entryOffsetBytes), &status)) {
		playbrew_log_read_failure(api, "entry header", &status, sizeof(entryOffsetBytes));
		return 0;
	}

	*payloadSize = playbrew_read_le32(payloadSizeBytes);
	*entryOffset = playbrew_read_le32(entryOffsetBytes);
	playbrew_log_header(api, *payloadSize, *entryOffset);
	return 1;
}

static int playbrew_read_payload_body(PlayBrewAPI *api, void *fh, uintptr_t payloadStartAddress, uint32_t payloadSize) {
	PlayBrewReadStatus status;

	if (!playbrew_read_exact(api, fh, (void *)payloadStartAddress, payloadSize, &status)) {
		api->printf("payload: body destination was 0x%08x. ", (unsigned int)payloadStartAddress);
		playbrew_log_read_failure(api, "body", &status, payloadSize);
		return 0;
	}

	api->printf("payload: body read finished (%u bytes). ",
		(unsigned int)payloadSize);
	return 1;
}

static int playbrew_payload_start(uint32_t payloadSize, uintptr_t *payloadStartAddress, const char **failureReason) {
	if (payloadSize == 0u) {
		*failureReason = "payload is empty";
		return 0;
	}

	uintptr_t payloadSizeAddress = (uintptr_t)payloadSize;
	if (payloadSizeAddress > UINTPTR_MAX - PLAYBREW_PAYLOAD_ALIGNMENT_MASK) {
		*failureReason = "payload size cannot be aligned";
		return 0;
	}

	uintptr_t alignedSize = (payloadSizeAddress + PLAYBREW_PAYLOAD_ALIGNMENT_MASK) & ~PLAYBREW_PAYLOAD_ALIGNMENT_MASK;
	if (alignedSize > PLAYBREW_MAX_PAYLOAD_SIZE) {
		*failureReason = "payload is larger than the loader limit";
		return 0;
	}

	uintptr_t startAddress = PLAYDATE_RAM_END - alignedSize;
	if (startAddress < PLAYBREW_SAFE_LOAD_REGION_START) {
		*failureReason = "payload would start outside the safe load region";
		return 0;
	}

	*payloadStartAddress = startAddress;
	return 1;
}

void playbrew_loader_load(void) {
	uint32_t payloadSize;
	uint32_t entryOffset;
	uintptr_t payloadStartAddress;
	const char *payloadFailureReason;
	
	PlayBrewAPI api;
	
	api.sd_open = (void *(*)(const char *, int))ADDR_SD_OPEN;
	api.sd_close = (void (*)(void *))ADDR_SD_CLOSE;
	api.sd_read = (int (*)(void *, void *, size_t))ADDR_SD_READ;
	api.printf = (void (*)(const char *, ...))ADDR_PRINTF;
	api.gfx_clear = (void (*)(int))ADDR_GFX_CLEAR;
	api.gfx_drawLine = (void (*)(int, int, int, int, int, int))ADDR_GFX_DRAWLINE;
	api.gfx_setPixel = (void (*)(int, int, int))ADDR_GFX_SETPIXEL;
	
	api.printf("started PlayBrew! ");

	void *log = api.sd_open(PLAYBREW_LOG_SMOKE_TEST_PATH, PLAYBREW_SD_MODE_WRITE);
	if (log != NULL) {
		api.sd_close(log);
	}

	void *fh = api.sd_open(PLAYBREW_PAYLOAD_PATH, PLAYBREW_SD_MODE_READ);
	
	if (fh == NULL) {
		api.printf("payload: open failed for %s. ", PLAYBREW_PAYLOAD_PATH);
		return;
	}
	
	if (!playbrew_read_payload_header(&api, fh, &payloadSize, &entryOffset)) {
		api.sd_close(fh);
		return;
	}

	if (!playbrew_payload_start(payloadSize, &payloadStartAddress, &payloadFailureReason)) {
		api.printf("payload: invalid size %u (%s). ", payloadSize, payloadFailureReason);
		api.sd_close(fh);
		return;
	}

	if (!playbrew_entry_offset_is_valid(payloadSize, entryOffset)) {
		playbrew_log_entry_failure(&api, payloadSize, entryOffset);
		api.sd_close(fh);
		return;
	}

	playbrew_log_load_details(&api, payloadStartAddress, payloadSize, entryOffset);
	
	if (!playbrew_read_payload_body(&api, fh, payloadStartAddress, payloadSize)) {
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
