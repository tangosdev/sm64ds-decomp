#include "private/card_backup_spec.h"

extern const struct CardBackupSpec data_020867bc[];

struct CardBackupSpec *func_02060398(int arg) {
    const struct CardBackupSpec *p = data_020867bc;
    for (;;) {
        if (p->type == arg) {
            return (struct CardBackupSpec *)p;
        }
        if (p->type == 0) {
            return 0;
        }
        p++;
    }
}

/* The backup specs, one per CARD backup type this cartridge supports; the
   zero-type entry ends the table. */
const struct CardBackupSpec data_020867bc[] = {
    {0x00000901, 0x00000200, {0x00, 0x02, 0x00, 0x00, 0x10, 0x00}, 0x0001, {0x4f}},
    {0x00000d01, 0x00002000, {0x00, 0x20, 0x00, 0x00, 0x20, 0x00}, 0x0002, {0x4f}},
    {0x00001202,
     0x00040000,
     {0x00, 0x00, 0x01, 0x00, 0x00, 0x01},
     0x0003,
     {0x4f, 0,    0,    0,    0x8a, 0x01, 0,    0,    0x70, 0x12, 0,
      0,    0x98, 0x34, 0x01, 0,    0x98, 0x34, 0x01, 0,    0x70, 0x12}},
    {0},
};
