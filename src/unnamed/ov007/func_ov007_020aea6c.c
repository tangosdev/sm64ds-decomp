#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Item.h"
#include "Ov007MenuMem.h"
#define func_ov007_020c9388 _Z19func_ov007_020c9388i
extern void *func_ov007_020c9388(int initialState);
extern void func_ov007_020ae810(Ov007Item *item);

/* Allocates an item: its id, layout row and hit box; position 0, fade and
 * slideOut at 0x1000, visible; then seeds its position from the
 * layout's home (func_ov007_020ae810). */
Ov007Item *func_ov007_020aea6c(int id, Ov007ItemLayout *layout, Ov007HitBox *hitBox) {
    Ov007Item *item = func_ov007_020c3df4(0, sizeof(Ov007Item));
    item->state = func_ov007_020c9388(1);
    item->id = id;
    item->half02 = 0;
    item->layout = layout;
    item->hitBox = hitBox;
    item->posY = 0;
    item->posX = item->posY;
    item->visible = 1;
    item->fade = 0x1000;
    item->slideOut = 0x1000;
    item->wiggleTimer = 0;
    func_ov007_020ae810(item);
    return item;
}
