#include "types.h"
extern void _ZN5Scene20SetAndStopColorFaderEv(void);

typedef struct WipeEntry {
    u32 pad0;
    u32 field4;
    u32 field8;
} WipeEntry;

typedef struct WipeTable {
    u8 pad[0x240];
    WipeEntry entries[1];
} WipeTable;

extern WipeTable* WIPES;                    /* 0x0209f324 */
extern WipeEntry* KS_FADER;                 /* 0x0209d4b0 */
extern u8 CAMERA_SAVED_LOOK_AT_RELATED;    /* 0x0209f274 */

void FUN_02029ab0(void) {
    WipeEntry* entry;
    _ZN5Scene20SetAndStopColorFaderEv();
    entry = &WIPES->entries[0];
    KS_FADER = entry;
    entry->field4 = 0xc00;
    entry->field8 = 0;
    CAMERA_SAVED_LOOK_AT_RELATED = 1;
}
