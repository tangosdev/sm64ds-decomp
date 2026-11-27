#include "private/card_backup_spec.h"

extern struct CardBackupSpec *func_02060398(int arg);

/* card_backup.c's selected-spec state: the table entry CARD_IdentifyBackup
   selected and its sector-size word. The remaining 0x18 bytes of the range
   have no reference in the ROM; one trailing word array keeps the exact
   tiling through 0x020a8780. */
struct CardBackupSpec *data_020a8760;
unsigned int data_020a8764;
unsigned int data_020a8768[6];

void func_02060364(int arg)
{
    struct CardBackupSpec *p = func_02060398(arg);
    data_020a8760 = p;
    data_020a8764 = p->h0e;
}
