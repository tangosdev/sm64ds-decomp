//cpp
#include "private/watchdog_alarm.h"

/* Link start. This is the only writer of data_0209e64c. The actor RNG
   data_0209e650 is the next bss word, so the seed lives here. Actors only
   read it. */

extern unsigned char data_0209e64c;
extern unsigned char data_0208f274;
extern char data_02075358[];
extern const char data_020752cc[28];
extern const char data_020752e8[28];
extern const char data_02075304[28];
extern const char data_02075320[28];
extern const char data_0207533c[28];
extern "C" {
extern void OS_GetOwnerInfo(unsigned char *dst);
}

extern "C" void func_0203db64(void *a, void *b);
extern "C" int func_0203daac(void);

void func_0201fec8(void)
{
    unsigned char buf[0x50];
    unsigned char saved;
    if (data_0209e64c != 0) return;
    saved = func_0201a9ec(&data_0209d574);
    OS_GetOwnerInfo(buf);
    switch (buf[0]) {
    case 2:
        func_0203db64(data_02075358, (char *)data_02075320);
        break;
    case 3:
        func_0203db64(data_02075358, (char *)data_0207533c);
        break;
    case 4:
        func_0203db64(data_02075358, (char *)data_020752cc);
        break;
    case 5:
        func_0203db64(data_02075358, (char *)data_020752e8);
        break;
    default:
        func_0203db64(data_02075358, (char *)data_02075304);
        break;
    }
    data_0208f274 = func_0203daac();
    func_0201a9fc(&data_0209d574);
    func_0201a96c(&data_0209d574, saved);
    data_0209e64c = 1;
}

/* One s32 seeded to 1. The empty destructor is func_0201973c, already
   defined in func_020196cc.c, so this declaration is not emitted. */
#ifndef SM64DS_PLATFORM_PC
extern "C" void func_0203b9b4(int *p, int v);
#include "LevelRng.h"

inline LevelRng::LevelRng() {
    func_0203b9b4(&value, 1);
}

LevelRng data_0209e650;
#endif
