#include "types.h"
extern void _ZN8dScene_c20SetAndStopColorFaderEv(void);

typedef struct WipeEntry {
    u32 pad0;
    u32 field4;
    u32 field8;
} WipeEntry;

typedef struct WipeTable {
    u8 pad[0x240];
    WipeEntry entries[1];
} WipeTable;

extern WipeTable* data_0209f324;                    /* 0x0209f324 */
extern WipeEntry* data_0209d4b0;                 /* 0x0209d4b0 */
extern u8 data_0209f274;    /* 0x0209f274 */

void FUN_02029ab0(void) {
    WipeEntry* entry;
    _ZN8dScene_c20SetAndStopColorFaderEv();
    entry = &data_0209f324->entries[0];
    data_0209d4b0 = entry;
    entry->field4 = 0xc00;
    entry->field8 = 0;
    data_0209f274 = 1;
}
