//cpp
#include "types.h"
#include "private/rom_header_view.h"
#include "private/mb_views.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "nitro/os/system.h"

extern "C" {

extern char *data_020a9db8;
extern u8 data_020a9d28;

/* Typed through a cast: the other declarations of data_020a9db8 spell it as
 * a byte pointer (MB_Parent.cpp's MB_WORK). */
#define MB_WORK ((MbParentWork *)data_020a9db8)

/* The work pointer stepped a whole MbParentFile per index, so that
 * ->mFiles[0] is file record i: the search loop builds its address this way
 * (MB_Parent.cpp's MB_FILE). &MB_WORK->mFiles[i] builds the loop 4 bytes
 * longer. */
#define MB_STEP(i) ((MbParentWork *)(data_020a9db8 + (i) * (int)sizeof(MbParentFile)))

/* The image's authentication code follows the 0x160-byte RomHeaderView and
 * the two static modules. */

extern int MBi_IsStarted(void);
extern void MBi_MakeDownloadFileInfo(void *out, void *in, void *dst);
extern int MBi_MakeBlockInfoTable(void *a, void *b);
extern void MBi_MakeGameInfo(void *dst, void *src, void *extra);
extern void MB_AddGameInfo(void *n);

/* NitroSDK MB_RegisterFile(game_reg, buf): file a game registry and its ROM
 * image in the first free mFiles slot (refusing a 17th file or the same
 * registry twice), build its download header, block table and game record,
 * add the record to the beacon list (MB_AddGameInfo) and mark the slot
 * active with only the parent entered. */
int MB_RegisterFile(void *a, void *b) {
    void *dst2;
    int idx;
    char *end;
    OSIntrMode state;
    u8 i;

    end = (char *)b + sizeof(RomHeaderView) + ((RomHeaderView *)b)->main.size +
          ((RomHeaderView *)b)->sub.size;
    idx = 0xff;

    state = OS_DisableInterrupts();
    if (!MBi_IsStarted()) {
        OS_RestoreInterrupts(state);
        return 0;
    }

    if (MB_WORK->mFileCount + 1 > 0x10) {
        OS_RestoreInterrupts(state);
        return 0;
    }

    for (i = 0; i < 0x10; i++) {
        MbParentWork *slot = MB_STEP(i);
        if (slot->mFiles[0].mGameRegistry == a) {
            OS_RestoreInterrupts(state);
            return 0;
        }
        if (slot->mFiles[0].mActive == 0) {
            idx = i;
            break;
        }
    }
    if (i == 0x10) {
        OS_RestoreInterrupts(state);
        return 0;
    }

    MB_WORK->mFiles[idx].mGameRegistry = (u8 *)a;

    dst2 = MB_WORK->mFiles[idx].mHeader;
    MBi_MakeDownloadFileInfo(dst2, b, end);

    if (!MBi_MakeBlockInfoTable(MB_WORK->mFiles[idx].mBlockTable, dst2)) {
        OS_RestoreInterrupts(state);
        return 0;
    }

    MBi_MakeGameInfo(&MB_WORK->mFiles[idx].mHeader[0xe4], a, MB_WORK->mParentUser);

    ((MBGameInfo *)&MB_WORK->mFiles[idx].mHeader[0xe4])->fileNo = (u8)idx;

    MB_AddGameInfo(&MB_WORK->mFiles[idx].mHeader[0xe4]);

    {
        u8 id = data_020a9d28;
        data_020a9d28 = id + 1;
        ((MBGameInfo *)&MB_WORK->mFiles[idx].mHeader[0xe4])->seqNoFixed = id;
    }

    MB_WORK->mFiles[idx].mEntryBitmap = 1;
    MB_WORK->mFiles[idx].mSrcAddr = (s32)b;
    MB_WORK->mFiles[idx].mActive = 1;
    MB_WORK->mFileCount += 1;

    OS_RestoreInterrupts(state);
    return 1;
}
}
