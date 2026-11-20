#include "nitro/fs/overlay.h"
#include "nitro/os/system.h"
extern int func_0203d7b8(void);

/* A loaded-overlay slot (see func_02017e60.c): the overlay id, -1 when the
   slot is free, and the RAM range the overlay occupies. */
struct OverlaySlot {
    int id;
    int start;
    int size;
};
extern struct OverlaySlot gLoadedOverlays[];

void UnloadOverlay(int id) {
    FSOverlayInfo info;
    struct OverlaySlot *found = 0;
    unsigned int i;
    struct OverlaySlot *r = gLoadedOverlays;
    for (i = 0; i < 12; i++, r++) {
        if (r->id == id) {
            r->id = -1;
            r->start = 0;
            found = r;
            r->size = 0;
        }
    }
    if (found == 0)
        OS_Terminate();
    FS_UnloadOverlay(MI_PROCESSOR_ARM9, id);
    func_02018c00(&info, 0, id);
    if (func_0203d7b8() != 0)
        return;
    func_02017fd0(&info);
}
