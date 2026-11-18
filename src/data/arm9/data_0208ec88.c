/* Filesystem (myFS) tables and the stage-entry name strings.
 * data_0208eb54..data_0208ecf4 are the fsi/file-handle records myFS_OpenFile
 * and myFS_ReadFile walk; 0x0208ecf4..0x0208ee50 is the __files archive table
 * and the per-stage name strings dScStage_c and dScEntry_c load.
 * ROM 0x0208ec88..0x0208ecc4, 60 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208ec9c: 20 bytes. */
char data_0208ec9c[20] = "/ARCHIVE/vs4.narc";

/* 0x0208ec88: 20 bytes. */
char data_0208ec88[20] = "/ARCHIVE/c2d.narc";

/* 0x0208ecb0: 20 bytes. */
char data_0208ecb0[20] = "/ARCHIVE/arc0.narc";
