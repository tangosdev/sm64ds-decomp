#include "types.h"
extern int func_ov007_020c3598(int a, int b, s8 *c, int d);
extern void func_ov007_020b8d48(void *obj, unsigned int v);
extern u8 data_ov007_020d7a38[];
extern int data_ov007_02103464;

void func_ov007_020b8b00(void *obj, int arg) {
    s8 buf[3];
    int i;
    int saved;
    int r;
    s8 *p;
    saved = data_ov007_02103464;
    r = func_ov007_020c3598(arg, 3, buf, 10);
    p = buf + 2;
    for (i = 2; i >= 0; i--) {
        if (*p >= 0)
            func_ov007_020b8d48(obj, data_ov007_020d7a38[*p]);
        p--;
    }
    data_ov007_02103464 = saved + r * 7;
}
