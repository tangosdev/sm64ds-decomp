//cpp
#include "private/watchdog_alarm.h"
#include "nitro/wm/wm.h"

/* Link start. This is the only writer of data_0209e64c. The actor RNG
   data_0209e650 is the next bss word, so the seed lives here. Actors only
   read it. */

extern unsigned char data_0209e64c;
extern unsigned char data_0208f274;
extern const WMParentParam data_02075358;
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
        func_0203db64((void *)&data_02075358, (char *)data_02075320);
        break;
    case 3:
        func_0203db64((void *)&data_02075358, (char *)data_0207533c);
        break;
    case 4:
        func_0203db64((void *)&data_02075358, (char *)data_020752cc);
        break;
    case 5:
        func_0203db64((void *)&data_02075358, (char *)data_020752e8);
        break;
    default:
        func_0203db64((void *)&data_02075358, (char *)data_02075304);
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

/* The parent parameters the download-play link start keeps: channel 1, up to
   three children of 0x20 bytes each under a 0x200-byte parent, a beacon every
   200 frames, group id 0x00800000. */
const WMParentParam data_02075358 = {
    0,     0,    {0, 0x80},       0, 1, 3, 0, 0, 1, 200, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 1,
    0x200, 0x20, {0, 0, 0, 0, 0},
};

/* The game introduction the multiboot child shows, one per language: Italian,
   Spanish, German, French and English. extern because the MBGameRegistry rows
   in .data reference them by name. */
extern const unsigned short data_02075398[48] = {
    'A', 'f', 'f',    'e', 'r', 'r', 'a', ' ',  'l', 'e', ' ', 'S', 't', 'e', 'l', 'l',
    'e', '!', 0x000a, 'I', ' ', 'c', 'a', 'p',  'p', 'e', 'l', 'l', 'i', ' ', 't', 'i',
    ' ', 's', 'o',    'n', 'o', ' ', 'd', '\'', 'a', 'i', 'u', 't', 'o', '.', 0,   0,
};

extern const unsigned short data_020753f8[50] = {
    0x00a1, 'H', 'a', 'z', 't', 'e', ' ', 'c', 'o',    'n', ' ', 'l', 'a',    's', ' ', 'E', 's',
    't',    'r', 'e', 'l', 'l', 'a', 's', '!', 0x000a, 'L', 'a', 's', ' ',    'g', 'o', 'r', 'r',
    'a',    's', ' ', 't', 'e', ' ', 'a', 'y', 'u',    'd', 'a', 'r', 0x00e1, 'n', '.', 0,
};

extern const unsigned short data_0207545c[52] = {
    'G', 'r', 'e', 'i', 'f', ' ',    'n', 'a', 'c', 'h', ' ', 'd',    'e', 'n', ' ', 'S', 't', 'e',
    'r', 'n', 'e', 'n', '!', 0x000a, 'D', 'i', 'e', ' ', 'M', 0x00fc, 't', 'z', 'e', ' ', 'h', 'i',
    'l', 'f', 't', ' ', 'd', 'i',    'r', ' ', 'd', 'a', 'b', 'e',    'i', '!', 0,   0,
};

extern const unsigned short data_020754c4[54] = {
    'B',    'a', 't', 's', '-', 't', 'o', 'i',  ' ',    'p', 'o', 'u', 'r', ' ', 'l', 'e', 's', ' ',
    0x00e9, 't', 'o', 'i', 'l', 'e', 's', '!',  0x000a, 'L', 'e', 's', ' ', 'c', 'a', 's', 'q', 'u',
    'e',    't', 't', 'e', 's', ' ', 't', '\'', 'a',    'i', 'd', 'e', 'r', 'o', 'n', 't', '!', 0,
};

extern const unsigned short data_02075530[56] = {
    'S', 'c', 'r', 'a', 'm', 'b', 'l', 'e', ' ', 't', 'o', ' ', 'g',    'e',
    't', ' ', 't', 'h', 'e', ' ', 'S', 't', 'a', 'r', 's', '!', 0x000a, 'T',
    'h', 'e', ' ', 'c', 'a', 'p', 's', ' ', 'w', 'i', 'l', 'l', ' ',    'h',
    'e', 'l', 'p', ' ', 'y', 'o', 'u', ' ', 'o', 'u', 't', '.', 0,      0,
};
