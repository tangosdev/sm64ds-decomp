/* Filesystem (myFS) tables and the stage-entry name strings.
 * data_0208eb54..data_0208ecf4 are the fsi/file-handle records myFS_OpenFile
 * and myFS_ReadFile walk; 0x0208ecf4..0x0208ee50 is the __files archive table
 * and the per-stage name strings dScStage_c and dScEntry_c load.
 * ROM 0x0208eb58..0x0208eb9c, 68 bytes, 14 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208eb88: 4 bytes. */
char data_0208eb88[4] = "ceg";

/* 0x0208eb84: 4 bytes. */
char data_0208eb84[4] = "vs4";

/* 0x0208eb80: 4 bytes. */
char data_0208eb80[4] = "cei";

/* 0x0208eb7c: 4 bytes. */
char data_0208eb7c[4] = "ar1";

/* 0x0208eb78: 4 bytes. */
char data_0208eb78[4] = "vs3";

/* 0x0208eb74: 4 bytes. */
char data_0208eb74[4] = "en1";

/* 0x0208eb70: 4 bytes. */
char data_0208eb70[4] = "vs1";

/* 0x0208eb6c: 4 bytes. */
char data_0208eb6c[4] = "ces";

/* 0x0208eb68: 4 bytes. */
char data_0208eb68[4] = "cee";

/* 0x0208eb64: 4 bytes. */
char data_0208eb64[4] = "cef";

/* 0x0208eb60: 4 bytes. */
char data_0208eb60[4] = "c2d";

/* 0x0208eb5c: 4 bytes. */
char data_0208eb5c[4] = "ar0";

/* 0x0208eb58: 4 bytes. */
char data_0208eb58[4] = "vs2";

/* 0x0208eb8c: 16 bytes. */
int data_0208eb8c[4] = {
    0x5346796d,
    0x65704f5f,
    0x6c69466e,
    0x00000065,
};
