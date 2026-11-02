#include "types.h"
#include "nitro/fs/overlay.h"
#include "private/fs_views.h"
int func_0203d7b8(void);
FSFileID FS_GetOverlayFileID(const FSOverlayInfo *p_ovi);
void Crash(void);
void FS_LoadOverlay(int a, int id);
/* A loaded-overlay slot (see func_02017e60.c): the overlay id, 0xffffffff
   when the slot is free, and the RAM range the overlay occupies. */
struct OverlaySlot {
    u32 id;
    u32 start;
    u32 size;
};
extern struct OverlaySlot data_0209d3c4[12];

void LoadOverlay(int id) {
    u32 start;
    struct OverlaySlot *slot;
    u32 i;
    u32 total;
    FSFileID pair;
    FSOverlayInfo info;
    struct OverlaySlot *r;
    u32 end;
    func_02018c00(&info, 0, id);
    if (func_0203d7b8() != 0) {
        return;
    }
    pair = FS_GetOverlayFileID(&info);
    slot = 0;
    total = info.header.ram_size + info.header.bss_size;
    start = (u32)info.header.ram_address;
    end = start + total;
    for (r = data_0209d3c4, i = 0; i < 12; r++, i++) {
        if (r->id == 0xffffffff) {
            if (slot == 0) {
                slot = r;
            }
        } else if (((r->start + r->size) > start) && (end > r->start)) {
            Crash();
        }
    }

    if (slot != 0) {
        slot->id = id;
        slot->start = start;
        slot->size = total;
    }
    FS_LoadOverlay(0, id);
    func_0203d7b8();
}
