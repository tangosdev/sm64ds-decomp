#include "nitro/fs/overlay.h"
extern void Crash(void);
extern int FS_UnloadOverlay(int a, int b);
extern int func_0203d7b8(void);

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
        Crash();
    FS_UnloadOverlay(0, id);
    func_02018c00(&info, 0, id);
    if (func_0203d7b8() != 0)
        return;
    func_02017fd0(&info);
}
