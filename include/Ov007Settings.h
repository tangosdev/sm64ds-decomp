#ifndef OV007_SETTINGS_H
#define OV007_SETTINGS_H

#include "types.h"

/* Reconstructed names and layout, not an original class identity. */

/* func_ov007_020b7138's second argument: dScDSMT_c's byte-packed
 * download-play table (data_0209b340). dScDSMT_c fills it before the menu
 * starts (func_ov007_020cc168, one save slot at a time) and reads bootFlags
 * back when it ends. */
typedef struct Ov007BootArgs {
    /* 0x00: bit 0 picks the first figure, bits 1-2 and 3-4 the two stroke
     * stages (func_ov007_020b7138); func_ov007_020b6f4c's on the way out */
    int bootFlags;
    /* 0x04: dScDSMT_c::InitResources stores 1 here (when gSoftResetPending is
     * set), CleanupResources 2 */
    int status;
    /* 0x08: the sound-mode option, 0..2; func_ov007_020cc168 maps
     * GetSoundMode 0/1/2 to 1/2/0, func_ov007_020b64a8 steps it and
     * func_ov007_020b7eec shows its message */
    u8 soundModeItem;
    /* 0x09: the backlight setting (data_0208ee3c[0]); picks which of menu
     * items 0xc and 0xd is live */
    u8 backlightOn;
    char pad_0a;
    u8 fileUsed[3]; /* 0x0b: per save file, flags1 bit 0 */
    /* 0x0e: per save file, the highest unlocked character (of 0..2) + 1, 0
     * when none is; func_ov007_020cc168's name */
    u8 highestUnlockedPlusOne[3];
    /* 0x11: per save file and course, the displayed star count less star 0 */
    u8 otherStars[3][15];
    u8 coinRecords[3][15]; /* 0x3e: per save file and course */
    u8 firstStar[3][15];   /* 0x6b: per save file and course, star 0 collected */
    u8 level1dStars[3];    /* 0x98: the displayed star count of level 0x1d */
    char pad_9b;
    /* 0x9c: dScDSMT_c::InitResources copies data_0209d6fc here, the
     * message file Message.cpp reads */
    void *messages;
} Ov007BootArgs;

#ifndef SM64DS_PLATFORM_PC
typedef char
    Ov007BootArgs_messages_must_be_at_0x9c[(int)&((Ov007BootArgs *)0)->messages == 0x9c ? 1 : -1];
/* It lives at data_0209b340 (data_0209b34b, 0209b34e and 0209b3d8 are its
 * fileUsed, highestUnlockedPlusOne and level1dStars). The next symbol past
 * it is 0xac bytes in, but nothing in ov007 reads past messages. */
typedef char Ov007BootArgs_size_must_be_0xa0[sizeof(Ov007BootArgs) == 0xa0 ? 1 : -1];
#endif

/* The 0x3c-byte record func_ov007_020b6a84 allocates (Ov007Work's
 * settings). */
typedef struct Ov007Settings {
    Ov007BootArgs *bootArgs; /* 0x00: func_ov007_020b7138's second argument */
    /* 0x04: per save file, its star total, -1 for an unused file
     * (func_ov007_020aec00) */
    s16 starCounts[3];
    char pad_0a[0x2d];
    /* 0x37 / 0x38: 1 when the owner's dispatcher (func_ov007_020cc600)
     * failed to read the save files (command 0x14) / the minigame data
     * (0x15), as func_ov007_020b066c stores them */
    u8 filesFailed;
    u8 minigamesFailed;
    /* 0x39: func_ov007_020b7d94's kind, the window menu state 9 opens before
     * the choice goes through (func_ov007_020b0834); 0 = none */
    u8 dialogKind;
    u8 language; /* 0x3a: func_ov007_020b6a84 seeds it from the owner-info language */
    char pad_3b;
} Ov007Settings;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007Settings_size_must_be_0x3c[sizeof(Ov007Settings) == 0x3c ? 1 : -1];
#endif

#endif
