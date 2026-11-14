#include "nitro/fs/overlay.h"
#include "nitro/os/system.h"
extern int _Z13func_0203d7b8v(void);

/* A loaded-overlay slot (see func_02017e60.c): the overlay id, -1 when the
   slot is free, and the RAM range the overlay occupies. */
struct OverlaySlot {
    int id;
    int start;
    int size;
};
extern struct OverlaySlot data_0209d3c4[];

void UnloadOverlay(int id) {
    FSOverlayInfo info;
    struct OverlaySlot *found = 0;
    unsigned int i;
    struct OverlaySlot *r = data_0209d3c4;
    for (i = 0; i < 0xc; i++, r++) {
        if (r->id == id) {
            r->id = -1;
            r->start = 0;
            found = r;
            r->size = 0;
        }
    }
    if (found == 0)
        _Z12OS_Terminatev();
    FS_UnloadOverlay(MI_PROCESSOR_ARM9, id);
    func_02018c00(&info, 0, id);
    if (_Z13func_0203d7b8v() != 0)
        return;
    _Z13func_02017fd0P13FSOverlayInfo(&info);
}
