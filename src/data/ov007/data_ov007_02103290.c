//cpp
/* 0x02103290: dScDSMT_c's six menu hooks in Ov007MenuHooks order.
 * alloc/free/query/fileLoad/fileRelease/callback, the order
 * dScDSMT_c::InitResources hands to func_ov007_020b7138. */
#include "types.h"

#pragma define_section sec_02103290 ".data.dsmtCallbacks" ".data.dsmtCallbacks" ".data.dsmtCallbacks" far_data
#pragma section sec_02103290 begin
typedef struct Ov007MenuHooks {
    void *(*alloc)(u32 size);
    void (*free)(void *ptr);
    void (*query)(void);
    void *(*fileLoad)(int fileId);
    void (*fileRelease)(void *file);
    u8 (*callback)(s32 command);
} Ov007MenuHooks;

/* The hooks are defined in d_s_dsmt.cpp, which documents this table; they are
 * declared here rather than included because no header owns Ov007MenuHooks. */
extern void *DSMT_MenuAlloc(u32 size);
extern void DSMT_MenuFree(void *ptr);
extern void DSMT_MenuMaxAllocSize(void);
extern void *DSMT_MenuLoadFile(int fileId);
extern void DSMT_MenuReleaseFile(void *file);
extern u8 DSMT_MenuCallback(s32 command);

Ov007MenuHooks data_ov007_02103290 = {
    DSMT_MenuAlloc,
    DSMT_MenuFree,
    DSMT_MenuMaxAllocSize,
    DSMT_MenuLoadFile,
    DSMT_MenuReleaseFile,
    DSMT_MenuCallback,
};
#pragma section sec_02103290 end
