#ifndef PLAYBREW_H
#define PLAYBREW_H

#include <stddef.h>

typedef struct {
	void *(*ucfs_open)(char *path, int mode);
	void (*ucfs_close)(void *fh);
	int (*ucfs_read)(void *fh, void *buf, size_t len);

	void (*printf)(const char *format, ...);
	char *(*strcpy)(char *dest, const char *src);

	void *(*malloc)(size_t size);

	void (*gfx_clear)(int color);
	void (*gfx_drawLine)(int x1, int x2, int y1, int y2, int width, int color);
	void (*gfx_setPixel)(int x, int y, int color);
} PlayBrewAPI;

// ALL addresses must end in an ODD number to run the function in thumb mode and NOT ARM MODE
// NOT DOING THAT MAKES THE DEVICE CRASH :(

// also in hexadecimal it goes 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a, b, c, d, e, f
// that means that a, c, and e are technically even numbers while b, d, and f are not

#define ADDR_UCFS_OPEN ((void *(*)(char *, int))0x2405078f)
#define ADDR_UCFS_CLOSE ((void (*)(void *))0x240507c5)
#define ADDR_UCFS_READ ((int (*)(void *, void *, size_t))0x240507ef)

#define ADDR_PRINTF ((void (*)(const char *, ...))0x240baf51)
#define ADDR_STRCPY ((char *(*)(char *, const char *))0x240bba9d)

#define ADDR_MALLOC ((void *(*)(size_t))0x240b9de5) 

#define ADDR_GFX_CLEAR ((void (*)(int))0x24055d5d)
#define ADDR_GFX_DRAWLINE ((void (*)(int, int, int, int, int, int))0x24055be5)
#define ADDR_GFX_SETPIXEL ((void (*)(int, int, int))0x24055901)


#endif
