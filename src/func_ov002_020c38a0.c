// @symbol func_ov002_020c38a0
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int a, int b, int cc, unsigned int d);
extern int Vec3_HorzLen(struct Vector3 *v);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void _Z15ApproachLinear2Rsss(short *r, short a, short b);

#define LA(p) ((int)(p))

void func_ov002_020c38a0(char *c)
{
    struct {
        struct Vector3 a;
        struct Vector3 d;
        struct Vector3 e;
        struct Vector3 b;
    } s;
    int hlen;
    int ang;
    unsigned char *p;

    s.a.x = 0; s.a.y = 0; s.a.z = 0x60000;
    s.b.x = 0; s.b.y = 0; s.b.z = 0x60000;
    func_ov002_020c37a4(c, &s.b);
    s.d.x = 0; s.d.y = 0; s.d.z = 0;
    switch (*(unsigned char *)(c + 0x6e5)) {
    case 0:
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xc1, 0, 0x1000, 0);
        *(unsigned char *)(c + 0x743) = 0;
        func_ov002_020c3148(c);
        *(int *)(c + 0x690) = -0x20000;
        p = (unsigned char *)LA(c + 0x6e5);
        *p = *p + 1;
        /* fallthrough */
    case 1:
        if (func_ov002_020c2fec(c, &s.d) != 0) {
            p = (unsigned char *)LA(c + 0x6e5);
            *p = *p + 1;
            return;
        }
        s.e.x = s.d.x - *(int *)(c + 0x5c);
        s.e.y = s.d.y - *(int *)(c + 0x60);
        s.e.z = s.d.z - *(int *)(c + 0x64);
        hlen = Vec3_HorzLen(&s.e);
        ang = _ZN4cstd5atan2E5Fix12IiES1_(s.e.x, s.e.z);
        *(int *)(c + 0x5c) = s.d.x;
        *(int *)(c + 0x60) = s.d.y;
        *(int *)(c + 0x64) = s.d.z;
        if ((s.e.x | s.e.z) == 0) {
            *(short *)(c + 0x8c) = 0;
            *(short *)(c + 0x8e) = 0x7fff;
            return;
        }
        *(short *)(c + 0x8c) = -_ZN4cstd5atan2E5Fix12IiES1_(s.e.y, hlen);
        _Z15ApproachLinear2Rsss((short *)(c + 0x90), 0x2000, 0x10);
        *(short *)(c + 0x8e) = ang;
        break;
    case 2:
        break;
    }
}
