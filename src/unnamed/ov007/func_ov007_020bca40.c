#include "Ov007Work.h"
#include "Ov007Display.h"

/* Shows (1) or hides (0) a set of the eight BG layers, marking each for
 * upload: 0 sets the full layout, 1 hides layer 1, 2 shows layer 0, 3 hides
 * layers 1-3 and shows 4-7. */
void func_ov007_020bca40(int sel) {
    Ov007BgLayer **layers = gMenuWork->display->bgLayers;
    Ov007BgLayer *l;
    switch (sel) {
    case 0:
        l = layers[0];
        l->visible = l->dirty = 0;
        l = layers[1];
        l->visible = l->dirty = 1;
        l = layers[2];
        l->visible = l->dirty = 1;
        l = layers[3];
        l->visible = l->dirty = 1;
        l = layers[4];
        l->visible = l->dirty = 0;
        l = layers[5];
        l->visible = l->dirty = 0;
        l = layers[6];
        l->visible = l->dirty = 1;
        l = layers[7];
        l->visible = l->dirty = 1;
        break;
    case 1:
        l = layers[1];
        l->visible = l->dirty = 0;
        break;
    case 2:
        l = layers[0];
        l->visible = l->dirty = 1;
        break;
    case 3:
        l = layers[2];
        l->visible = l->dirty = 0;
        l = layers[3];
        l->visible = l->dirty = 0;
        l = layers[4];
        l->visible = l->dirty = 1;
        l = layers[5];
        l->visible = l->dirty = 1;
        l = layers[6];
        l->visible = l->dirty = 1;
        l = layers[7];
        l->visible = l->dirty = 1;
        l = layers[1];
        l->visible = l->dirty = 0;
        break;
    }
}
