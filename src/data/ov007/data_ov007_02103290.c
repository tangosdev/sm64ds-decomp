//cpp
/* 0x02103290: dScDSMT_c's six menu hooks in Ov007MenuHooks order.
 * Ov007MenuHooks, the six hooks in alloc/free/query/
 * fileLoad/fileRelease/callback order; the consumer is
 * dScDSMT_c::InitResources (src/game/stages/d_s_dsmt.cpp). */
#include "types.h"

#pragma define_section sec_02103290 ".data.dsmtCallbacks" ".data.dsmtCallbacks" ".data.dsmtCallbacks" far_data
#pragma section sec_02103290 begin
extern void *DSMT_MenuAlloc(u32 size);
extern void DSMT_MenuFree(void *ptr);
extern void DSMT_MenuMaxAllocSize(void);
extern void *DSMT_MenuLoadFile(int fileId);
extern void DSMT_MenuReleaseFile(void *file);
extern u8 DSMT_MenuCallback(s32 command);
typedef struct Ov007MenuHooks {
    int alloc;
    int free;
    int query;
    int fileLoad;
    int fileRelease;
    int (*callback)(int id);
} Ov007MenuHooks;
Ov007MenuHooks data_ov007_02103290 = {
    (int)&DSMT_MenuAlloc,
    (int)&DSMT_MenuFree,
    (int)&DSMT_MenuMaxAllocSize,
    (int)&DSMT_MenuLoadFile,
    (int)&DSMT_MenuReleaseFile,
    (int (*)(int))DSMT_MenuCallback,
};
#pragma section sec_02103290 end
