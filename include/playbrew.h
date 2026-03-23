#include <stddef.h>

typedef struct {
	void *(*sd_open)(const char *, int);
	void (*sd_close)(void *);
	void *(*sd_read)(const char *, void *, size_t);
	int (*printf)(const char *format, ...);
} PlayBrewAPI;

