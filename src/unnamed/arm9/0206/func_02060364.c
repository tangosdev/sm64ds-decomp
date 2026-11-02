#include "private/card_backup_spec.h"

extern struct CardBackupSpec *func_02060398(int arg);
extern struct CardBackupSpec *data_020a8760;
extern unsigned int data_020a8764;

void func_02060364(int arg)
{
    struct CardBackupSpec *p = func_02060398(arg);
    data_020a8760 = p;
    data_020a8764 = p->h0e;
}
