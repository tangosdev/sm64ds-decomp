#include "types.h"
#include "ds_width.h"
extern void LoadOverlay(int id);
extern int OVERLAY_6_ID;
extern int OVERLAY_4_ID;

/* Load overlay id, if any (-1 is none); overlay 6 loads overlay 4 first. */
void func_0201a798(int id) {
    if (id == -1) return;
    if (id == DS_IADDR(&OVERLAY_6_ID)) LoadOverlay(DS_IADDR(&OVERLAY_4_ID));
    LoadOverlay(id);
}
