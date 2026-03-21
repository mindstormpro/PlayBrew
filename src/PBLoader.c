//ARM GCC trunk (linux)
//ARGS:
//-mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16 
//-D__FPU_USED=1-fomit-frame-pointer -fno-common -fPIC

#define NULL ((void *)0)

//functions for 3.0.2 -- subject to change

typedef struct {
    void *(*sd_open)(const char *, int); //file path
    void (*sd_close)(void *); //file handle
    void *(*sd_read)(const char *, int *, int); //file handle, buffer, bytes

    void (*gfx_clear)(int); //color
    void (*gfx_drawLine)(int, int, int, int, int, int); //x1, y1, x2, y2, width, color
    void (*gfx_setPixel)(int, int, int) //x, y, color 

    int (*printf)(const char *__format, ...);
} ApiTable;

static ApiTable api;

void (*entry)(ApiTable *);

const void *ogCall = (void *)0x240c5dbb;
const int kFileRead = 3;
int payloadStart;
int entryPoint;
const int ramEnd = 0x91000000;

void myActualUpdateFunc(void) {
    api.sd_open  = (void *(*)(const char *, int))0x24067594;
    api.sd_close = (void (*)(void *))0x240676fc;
    api.sd_read  = (void *(*)(const char *, int *, int))0x24067918;
    api.gfx_clear = (void (*)(int))0x24055d5c;
    api.gfx_drawLine = (void (*)(int, int, int, int, int, int))0x24055be5;
    api.gfx_setPixel = (void (*)(int, int, int))0x24055901;
    api.printf = (int (*)(const char *, ...))0x240baf50;

    void *fh = api.sd_open("/payload.bin", 0x80 | kFileRead);
    if (fh == NULL) {
        return;
    }
    api.printf("loaded /payload.bin!");
    int filesz;
    api.sd_read(fh, &filesz, sizeof(int));
    api.sd_read(fh, &entryPoint, sizeof(int));

    if (filesz % 4 != 0) {
        payloadStart = filesz + 4 - (filesz % 4);
    } else {
        payloadStart = filesz;
    }
    payloadStart = ramEnd - payloadStart;

    api.sd_read(fh, (void *)payloadStart, filesz);

    int realEntryPoint = payloadStart + entryPoint;

    entry = (void (*)(ApiTable *))((unsigned int)realEntryPoint | 1);
    api.sd_close(fh);
    
    entry(&api);

}

void myUpdateFunc(void) __attribute__((naked));

void myUpdateFunc(void) {
  myActualUpdateFunc();
  goto *ogCall;
}