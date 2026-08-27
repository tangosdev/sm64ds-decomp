#include "types.h"
extern void UnloadOverlay(int id);
extern int overlay_6;
extern int overlay_4;

void func_0201a754(int id) {
    if (id == -1) return;
    UnloadOverlay(id);
    if (id != (int)&overlay_6) return;
    UnloadOverlay((int)&overlay_4);
}
