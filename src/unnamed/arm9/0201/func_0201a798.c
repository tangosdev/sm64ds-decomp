#include "types.h"
extern void LoadOverlay(int id);
extern int overlay_6;
extern int overlay_4;

void func_0201a798(int id) {
    if (id == -1) return;
    if (id == (int)&overlay_6) LoadOverlay((int)&overlay_4);
    LoadOverlay(id);
}
