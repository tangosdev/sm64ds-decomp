#include "MWCPlusLib.h"

/* The exception index table: data_020737a8 up to the start of .init. */
extern struct EHIndexEntry data_020737a8;
/* __sinit_d_camera.cpp, the first contribution in arm9 .init. The C lexer
   cannot spell that dotted linker name; ARM9_INIT_START is that base. */
extern void ARM9_INIT_START(void);

/* EH runtime: give func_02072dac the bounds of the one exception index
   table and return 1. The code address it also passes (pc) would pick a
   module's table; this image has one, so it goes unread. */
int func_02071844(struct EHTableInfo *info, int pc) {
    info->indexBegin = &data_020737a8;
    info->indexEnd = (struct EHIndexEntry *)ARM9_INIT_START;
    return 1;
}
