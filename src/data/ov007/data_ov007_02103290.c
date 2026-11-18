//cpp
/* 0x02103290: the hooks dScDSMT_c hands the menu, in struct Ov007MenuHooks
 * order (alloc/free/query/fileLoad/fileRelease/callback). */
#include "types.h"
#include "Ov007Settings.h"

#pragma define_section sec_02103290 ".data.dsmtCallbacks" ".data.dsmtCallbacks" ".data.dsmtCallbacks" far_data
#pragma section sec_02103290 begin
/* The hooks are defined in d_s_dsmt.cpp; declared here because no header owns
 * the DSMT_* helpers. The four allocator/file slots are the int words the
 * menu's hook installers store. */
extern void *DSMT_MenuAlloc(u32 size);
extern void DSMT_MenuFree(void *ptr);
extern void DSMT_MenuMaxAllocSize(void);
extern void *DSMT_MenuLoadFile(int fileId);
extern void DSMT_MenuReleaseFile(void *file);
extern u8 DSMT_MenuCallback(s32 command);

struct Ov007MenuHooks data_ov007_02103290 = {
    (int)&DSMT_MenuAlloc,
    (int)&DSMT_MenuFree,
    (int)&DSMT_MenuMaxAllocSize,
    (int)&DSMT_MenuLoadFile,
    (int)&DSMT_MenuReleaseFile,
    (int (*)(int))DSMT_MenuCallback,
};
#pragma section sec_02103290 end
