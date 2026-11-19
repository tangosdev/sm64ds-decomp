/* arm9 .bss 0x020a80c4..0x020a80c8: overlay-table mode for the SHA-1
 * block transform. DGT_SetOverlayTableMode reads and writes it;
 * SHA1ProcessMessageBlock tests it. A definition in the function file
 * does not survive function isolation, so the word lives here. */
#include "types.h"

int data_020a80c4;
