#include "types.h"
extern void UnloadOverlay(int id);
extern int OVERLAY_6_ID;
extern int OVERLAY_4_ID;

void func_0201a754(int id) {
    if (id == -1) return;
    UnloadOverlay(id);
    if (id != (int)&OVERLAY_6_ID) return;
    UnloadOverlay((int)&OVERLAY_4_ID);
}
