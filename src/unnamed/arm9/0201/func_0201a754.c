#include "types.h"
#include "ds_width.h"
extern void UnloadOverlay(int id);
extern int OVERLAY_6_ID;
extern int OVERLAY_4_ID;

/* Unload overlay id, if any (-1 is none); overlay 6 takes overlay 4 out
   with it. */
void func_0201a754(int id) {
    if (id == -1) return;
    UnloadOverlay(id);
    if (id != DS_IADDR(&OVERLAY_6_ID)) return;
    UnloadOverlay(DS_IADDR(&OVERLAY_4_ID));
}
