/* Filesystem (myFS) tables and the stage-entry name strings.
 * data_0208eb54..data_0208ecf4 are the fsi/file-handle records myFS_OpenFile
 * and myFS_ReadFile walk; 0x0208ecf4..0x0208ee50 is the __files archive table
 * and the per-stage name strings dScStage_c and dScEntry_c load.
 * ROM 0x0208ee04..0x0208ee14, 16 bytes, 1 configured symbols.
 */
#include "types.h"

extern u32 _ZTIN8dGraph_c10callback_cE[4];
extern u8 _ZTVN3abi17__class_type_infoE[];
extern char _ZTSN8dGraph_c10callback_cE[];

/* 0x0208ee04: 16 bytes. */
u32 _ZTIN8dGraph_c10callback_cE[4] = {
    (u32)_ZTVN3abi17__class_type_infoE,
    (u32)_ZTSN8dGraph_c10callback_cE,
    0x00000000,
    (u32)_ZTIN8dGraph_c10callback_cE,
};
