#include "Ov007BgLayer.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c05e4(Ov007BgList *list);

/* Builds an empty BG layer list for `engines` and `mask`. */
Ov007BgList *func_ov007_020c0604(int engines, int mask) {
    Ov007BgList *list = func_ov007_020c3df4(0, sizeof(Ov007BgList));
    func_ov007_020c05e4(list);
    list->engines = engines;
    list->mask = mask;
    return list;
}
