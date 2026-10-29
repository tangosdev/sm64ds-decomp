#include "Ov007Work.h"

extern void *func_ov007_020b3f20(int n);
extern void *func_ov007_020b2308(void *a, int b);

extern struct Ov007Work *data_ov007_0210342c;
void func_ov007_020b697c(void) {
    int i;
    void *n = func_ov007_020b3f20(0x16);
    for (i = 0; i < 0xb; i++) {
        void *r = func_ov007_020b2308(n, i);
        data_ov007_0210342c->textures[i] = r;
    }
}
