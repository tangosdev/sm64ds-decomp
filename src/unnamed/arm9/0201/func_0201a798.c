#include "types.h"
extern void LoadOverlay(int id);
extern int OVERLAY_6_ID;
extern int OVERLAY_4_ID;

void func_0201a798(int id) {
    if (id == -1) return;
    if (id == (int)&OVERLAY_6_ID) LoadOverlay((int)&OVERLAY_4_ID);
    LoadOverlay(id);
}
