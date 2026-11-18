/* Filesystem (myFS) tables and the stage-entry name strings.
 * data_0208eb54..data_0208ecf4 are the fsi/file-handle records myFS_OpenFile
 * and myFS_ReadFile walk; 0x0208ecf4..0x0208ee50 is the __files archive table
 * and the per-stage name strings dScStage_c and dScEntry_c load.
 * ROM 0x0208ebac..0x0208ec74, 200 bytes, 10 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208ec4c: 20 bytes. */
char data_0208ec4c[20] = "/ARCHIVE/ces.narc";

/* 0x0208ec38: 20 bytes. */
char data_0208ec38[20] = "/ARCHIVE/vs3.narc";

/* 0x0208ec24: 20 bytes. */
char data_0208ec24[20] = "/ARCHIVE/cef.narc";

/* 0x0208ec10: 20 bytes. */
char data_0208ec10[20] = "/ARCHIVE/vs2.narc";

/* 0x0208ebfc: 20 bytes. */
char data_0208ebfc[20] = "/ARCHIVE/vs1.narc";

/* 0x0208ebe8: 20 bytes. */
char data_0208ebe8[20] = "/ARCHIVE/cee.narc";

/* 0x0208ebd4: 20 bytes. */
char data_0208ebd4[20] = "/ARCHIVE/en1.narc";

/* 0x0208ebc0: 20 bytes. */
char data_0208ebc0[20] = "/ARCHIVE/cei.narc";

/* 0x0208ebac: 20 bytes. */
char data_0208ebac[20] = "/ARCHIVE/ar1.narc";

/* 0x0208ec60: 20 bytes. */
char data_0208ec60[20] = "/ARCHIVE/ceg.narc";
