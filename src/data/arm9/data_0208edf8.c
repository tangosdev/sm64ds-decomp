/* Filesystem (myFS) tables and the stage-entry name strings.
 * data_0208eb54..data_0208ecf4 are the fsi/file-handle records myFS_OpenFile
 * and myFS_ReadFile walk; 0x0208ecf4..0x0208ee50 is the __files archive table
 * and the per-stage name strings dScStage_c and dScEntry_c load.
 * ROM 0x0208edf8..0x0208ee00, 8 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x0208edf8: 8 bytes. */
int data_0208edf8[2] = {
    0x37375a4c,
    0x00000000,
};
