#include "nitro/fs/overlay.h"
extern int FS_UnloadOverlay(int a, int b);
extern int func_0203d7b8(void);

/* A loaded-overlay slot (see func_02017e60.c). */
struct OverlaySlot {
    int id;
    int start;
    int size;
};
extern struct OverlaySlot data_0209d3c4[];

/* UnloadOverlay for an overlay that may not be loaded: free its slot, and if
   it had one, unload ARM9 overlay `key` and, unless func_0203d7b8 says
   otherwise, poison its RAM (func_02017fd0). Where UnloadOverlay crashes on
   an overlay with no slot, this returns. */
void func_02017e94(int key) {
    FSOverlayInfo info;
    struct OverlaySlot *found = 0;
    unsigned int i;
    for (i = 0; i < 0xc; i++) {
        struct OverlaySlot *e = &data_0209d3c4[i];
        if (e->id == key) {
            e->id = -1;
            e->start = 0;
            found = e;
            e->size = 0;
        }
    }
    if (found == 0)
        return;
    FS_UnloadOverlay(0, key);
    func_02018c00(&info, 0, key);
    if (func_0203d7b8() != 0)
        return;
    func_02017fd0(&info);
}
