#include "Ov007Work.h"
#include "Ov007Display.h"

extern void func_ov007_020b283c(void);
extern void func_ov007_020b76a0(void);
extern void func_ov007_020bcba8(int i);
extern void func_ov007_020bca40(int i);
extern void func_ov007_020bd1c0(int i);
extern void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern void func_ov007_020aec00(void);
extern void func_ov007_020c1d78(int i);

void func_ov007_020af434(void) {
    Ov007BgLayer **layers;
    Ov007BgLayer *a;
    WORK->callback(0x12);
    func_ov007_020b283c();
    func_ov007_020b76a0();
    func_ov007_020bcba8(3);
    func_ov007_020bca40(3);
    func_ov007_020bd1c0(2);
    RequestScreenFade(1, 2, 0);
    func_ov007_020aec00();
    layers = WORK->display->bgLayers;
    a = layers[6];
    a->scrollX = 0;
    a->scrollY = 0;
    a->fineScrollY = 0;
    a->fineScrollX = a->fineScrollY;
    a = layers[7];
    a->scrollX = 0;
    a->scrollY = 0;
    a->fineScrollY = 0;
    a->fineScrollX = a->fineScrollY;
    func_ov007_020c1d78(0);
}
