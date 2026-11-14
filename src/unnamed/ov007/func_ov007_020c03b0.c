#include "Ov007BgLayer.h"
#include "nitro/gx/gx_bgcnt.h"

/* Per BG, its bit in Ov007BgList.mask. */
extern int data_ov007_020d7d6c[];
extern void func_ov007_020c076c(Ov007BgLayer *layer, int engines);

/* Shows the list's layers on its engines: for each BG, the first listed
 * layer with a valid BG and priority that is visible and in the mask is
 * uploaded (func_ov007_020c076c) and its priority written into BGnCNT. */
void func_ov007_020c03b0(Ov007BgList *list) {
    Ov007BgLayer *volatile layers[4];
    int shown[4];
    Ov007BgLayer *volatile *pa;
    int *pb;
    Ov007BgLayer *node;
    int mask;
    int bg, pri;
    int toMain, toSub;
    int engines;

    /* layers is stored but never read. Both arrays are cleared through
     * pointers; direct stores do not match. */
    pa = (Ov007BgLayer *volatile *)(layers);
    pa[0] = 0;
    pa[1] = 0;
    pa[2] = 0;
    pa[3] = 0;
    pb = (int *)(shown);
    pb[0] = 0;
    pb[1] = 0;
    pb[2] = 0;
    pb[3] = 0;

    engines = list->engines;
    node = list->head;
    mask = list->mask;
    toMain = engines & 1;
    toSub = engines & 2;
    if (node == 0)
        return;

    do {
        bg = node->bg;
        pri = node->priority;
        if (bg >= 0 && bg <= 3 && pri >= 0 && pri <= 3 && shown[bg] == 0) {
            layers[bg] = node;
            if (node->visible == 1 && (mask & data_ov007_020d7d6c[bg]) != 0) {
                func_ov007_020c076c(node, list->engines);
                if (toMain) {
                    switch (bg) {
                    case 0:
                        G2_SetBG0Priority(pri);
                        break;
                    case 1:
                        G2_SetBG1Priority(pri);
                        break;
                    case 2:
                        G2_SetBG2Priority(pri);
                        break;
                    case 3:
                        G2_SetBG3Priority(pri);
                        break;
                    }
                }
                if (toSub) {
                    switch (bg) {
                    case 0:
                        G2S_SetBG0Priority(pri);
                        break;
                    case 1:
                        G2S_SetBG1Priority(pri);
                        break;
                    case 2:
                        G2S_SetBG2Priority(pri);
                        break;
                    case 3:
                        G2S_SetBG3Priority(pri);
                        break;
                    }
                }
                shown[bg] = 1;
            }
        }
        node = node->next;
    } while (node != 0);
}
