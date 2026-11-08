#include "Ov007Work.h"

extern void func_ov007_020b7384(struct Ov007Display *d);
extern void func_ov007_020b7364(struct Ov007Display *d);

/* The display hook func_ov007_020b6eb4 calls: func_ov007_020b7384 loads the
 * display record's two OAM buffers and func_ov007_020b7364 uploads its main
 * and sub BG layer lists. */
void Display_Upload(void) {
    func_ov007_020b7384(WORK->display);
    func_ov007_020b7364(WORK->display);
}
