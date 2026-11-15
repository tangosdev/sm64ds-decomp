#ifndef S82_H
#define S82_H

#include "nitro/fs/archive.h"

struct NarcFatBlock;

/* A mounted .narc archive. The workers are defined in SoundEngine.cpp. */
struct S82 {
    /* 0x00 */ FSArchive arc;
    /* 0x50 */ char *narc;
    /* 0x54 */ NarcFatBlock *f54;
    /* 0x58 */ unsigned char *f58;
    int func_0204ede8(unsigned int i);
    int func_0204ee10();
    int func_0204ee40(char *name, char *p);
};

#endif
