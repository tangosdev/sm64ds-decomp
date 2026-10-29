//cpp
#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "IRQ.h"

extern "C" {

extern char *data_020a9db8;
extern u8 data_020a9d28;

/* Typed through a cast: the other declarations of data_020a9db8 spell it as
 * a byte pointer (func_02066a94.c's MB_WORK). */
#define MB_WORK ((MbParentWork *)data_020a9db8)

/* The work pointer stepped a whole MbParentFile per index, so that
 * ->mFiles[0] is file record i: the search loop builds its address this way
 * (func_02066a94.c's MB_FILE). &MB_WORK->mFiles[i] builds the loop 4 bytes
 * longer. */
#define MB_STEP(i) ((MbParentWork *)(data_020a9db8 + (i) * (int)sizeof(MbParentFile)))

/* NitroSDK RomHeader, the two static modules' load regions (the same view as
 * func_020676e0.c's): the image's authentication code follows the 0x160-byte
 * header and the two static modules. */
typedef struct RomStaticRegion {
    u32 ram_address; /* 0x00 */
    u32 size;        /* 0x04 */
} RomStaticRegion;

typedef struct RomHeaderView {
    u8 pad_00[0x28];
    RomStaticRegion main; /* 0x28 -- ARM9 static */
    u8 pad_30[0x38 - 0x30];
    RomStaticRegion sub; /* 0x38 -- ARM7 static */
} RomHeaderView;

/* The NitroSDK MBGameInfo tail written here (func_02068680.c's view names
 * the fields before it): seqNoFixed takes the next value of the counter at
 * data_020a9d28, fileNo the file's mFiles index. The record sits at
 * mHeader + 0xe4, as func_02066a94.c reaches it. */
typedef struct MBGameInfo {
    u8 pad_000[0x4ab];
    u8 seqNoFixed; /* 0x4ab */
    u8 seqNoVolat; /* 0x4ac -- func_0206853c.c */
    u8 fileNo;     /* 0x4ad */
} MBGameInfo;

extern int func_020687f8(void);
extern void func_020678a4(void *out, void *in, void *dst);
extern int func_02067604(void *a, void *b);
extern void func_02068680(void *dst, void *src, void *extra);
extern void func_020684d0(void *n);

#pragma opt_strength_reduction off

/* NitroSDK MB_RegisterFile(game_reg, buf): file a game registry and its ROM
 * image in the first free mFiles slot (refusing a 17th file or the same
 * registry twice), build its download header, block table and game record,
 * add the record to the beacon list (func_020684d0) and mark the slot
 * active with only the parent entered. */
int func_02067928(void *a, void *b)
{
    void *dst2;
    int idx;
    char *end;
    u32 state;
    u8 i;

    end = (char *)b + 0x160 + ((RomHeaderView *)b)->main.size + ((RomHeaderView *)b)->sub.size;
    idx = 0xff;

    state = IRQ::Disable();
    if (!func_020687f8()) {
        IRQ::Restore(state);
        return 0;
    }

    if (MB_WORK->mFileCount + 1 > 0x10) {
        IRQ::Restore(state);
        return 0;
    }

    for (i = 0; i < 0x10; i++) {
        MbParentWork *slot = MB_STEP(i);
        if (slot->mFiles[0].mGameRegistry == a) {
            IRQ::Restore(state);
            return 0;
        }
        if (slot->mFiles[0].mActive == 0) {
            idx = i;
            break;
        }
    }
    if (i == 0x10) {
        IRQ::Restore(state);
        return 0;
    }

    MB_WORK->mFiles[idx].mGameRegistry = (u8 *)a;

    dst2 = MB_WORK->mFiles[idx].mHeader;
    func_020678a4(dst2, b, end);

    if (!func_02067604(MB_WORK->mFiles[idx].mBlockTable, dst2)) {
        IRQ::Restore(state);
        return 0;
    }

    func_02068680(&MB_WORK->mFiles[idx].mHeader[0xe4], a, MB_WORK->mParentUser);

    ((MBGameInfo *)&MB_WORK->mFiles[idx].mHeader[0xe4])->fileNo = (u8)idx;

    func_020684d0(&MB_WORK->mFiles[idx].mHeader[0xe4]);

    {
        u8 id = data_020a9d28;
        data_020a9d28 = id + 1;
        ((MBGameInfo *)&MB_WORK->mFiles[idx].mHeader[0xe4])->seqNoFixed = id;
    }

    MB_WORK->mFiles[idx].mEntryBitmap = 1;
    MB_WORK->mFiles[idx].mSrcAddr = (s32)b;
    MB_WORK->mFiles[idx].mActive = 1;
    MB_WORK->mFileCount += 1;

    IRQ::Restore(state);
    return 1;
}
}
