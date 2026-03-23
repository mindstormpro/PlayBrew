#include <stddef.h>

typedef struct {
	void *(*sd_open)(const char *, int); //path, mode, returns file handle as a void ptr
	void (*sd_close)(void *); // file handle
	void *(*sd_read)(const char *, void *, size_t); //file handle, ptr to buffer, how much to read in bytes
	int (*printf)(const char *format, ...);
	void (*gfx_clear)(int); //color
    void (*gfx_drawLine)(int, int, int, int, int, int); //x1, y1, x2, y2, width, color
    void (*gfx_setPixel)(int, int, int) //x, y, color 
} PlayBrewAPI;

