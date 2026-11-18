/* Filesystem (myFS) tables and the stage-entry name strings.
 * data_0208eb54..data_0208ecf4 are the fsi/file-handle records myFS_OpenFile
 * and myFS_ReadFile walk; 0x0208ecf4..0x0208ee50 is the __files archive table
 * and the per-stage name strings dScStage_c and dScEntry_c load.
 * ROM 0x0208ee04..0x0208ee14, 16 bytes, 1 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__class_type_info 0x0209a774); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern u32 _ZTIN8dGraph_c10callback_cE[4];
extern char _ZTSN8dGraph_c10callback_cE[];

/* 0x0208ee04: 16 bytes. */
u32 _ZTIN8dGraph_c10callback_cE[4] = {
    0x0209a774,
    (u32)_ZTSN8dGraph_c10callback_cE,
    0x00000000,
    (u32)_ZTIN8dGraph_c10callback_cE,
};
