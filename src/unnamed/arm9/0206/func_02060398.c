#include "private/card_backup_spec.h"

extern struct CardBackupSpec data_020867bc[];

struct CardBackupSpec *func_02060398(int arg) {
    struct CardBackupSpec *p = data_020867bc;
    for (;;) {
        if (p->type == arg) {
            return p;
        }
        if (p->type == 0) {
            return 0;
        }
        p++;
    }
}
