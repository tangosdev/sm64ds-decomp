//cpp
#include "types.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"

extern "C" void func_ov007_020b1390(int mode);

/* func_ov007_020b11bc's second tick: func_ov007_020b1390 in mode 0, then
 * the file-select header redrawn from the boot arguments
 * (func_ov007_020b7eec). */
extern "C" void func_ov007_020b12e8(void) {
    func_ov007_020b1390(0);
    WORK->settings->bootArgs->func_ov007_020b7eec();
}
