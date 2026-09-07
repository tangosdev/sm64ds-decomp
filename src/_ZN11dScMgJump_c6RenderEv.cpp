//cpp
// @symbol _ZN11dScMgJump_c6RenderEv
#include "types.h"
#include "decl_common.h"
#include "dScMgJump_c.h"
/* dScMgJump_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable: the third of the three slots where this class's
 * table differs from dScMgD3DBase_c's.
 *
 * Draws the language-dependent instruction rows on the touch screen while
 * unk_4664 == 1, then sets up the 3D engine's fog registers twice -- once for the
 * scene, once with bit 30 set for the pass that follows -- and hands the model at
 * 0x501c its transform. 0x040004c8 / 0x040004cc are memory-mapped registers, which
 * is why the stores are volatile; that is I/O, not codegen steering.
 *
 * WAS A C99 FILE, so the declarations move inside `extern "C"`; in C++ they would
 * mangle and resolve to nothing. */

extern "C" {
typedef struct { int w[12]; } M48;
typedef struct { int w[3]; } V3;

void func_ov004_020b1e34(void *a, int b, int c, int d);
int GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void func_0203cd80(int *m, short angle);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);

extern int data_020a0e68;
}

s32 dScMgJump_c::Render()
{
    void *self = (void *)this;
    char *c = (char *)self;

    if (*(u16 *)(c + 0x4664) == 1) {
        int count;
        int a1v;
        int i;

        func_ov004_020b1e34(self, 0xe0, 0x14, 1);

        count = data_ov006_02140428;
        a1v = 0x6e;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)data_ov006_0213cbb4[idx][1], (void *)a1v, 0xc, (void *)0);
            } else {
                RenderOamMainScreen(data_ov006_02134cf8, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    {
        int m[3];
        m[0] = 0;
        m[1] = 0;
        m[2] = 0xfffff008;
        func_0203cd80(m, -0x2000);
        *(volatile int *)0x40004c8 =
            (((short)m[0] >> 3) & 0x3ff) |
            ((((short)m[1] >> 3) & 0x3ff) << 10) |
            ((((short)m[2] >> 3) & 0x3ff) << 20);
        *(volatile int *)0x40004cc = 0x7fff;
        *(volatile int *)0x40004cc = 0x40007fff;
        Matrix4x3_FromTranslation(&data_020a0e68, 0, -0xef000, 0);
    }
    {
        int m[3];
        m[0] = 0;
        m[1] = 0;
        m[2] = 0xfffff008;
        func_0203cd80(m, -0x2000);
        *(volatile int *)0x40004c8 =
            ((((short)m[0] >> 3) & 0x3ff) |
            ((((short)m[1] >> 3) & 0x3ff) << 10) |
            ((((short)m[2] >> 3) & 0x3ff) << 20)) | 0x40000000;
    }

    *(M48 *)(c + 0x5038) = *(M48 *)&data_020a0e68;

    {
        V3 t;
        void *obj;
        void *p;

        t = *(V3 *)data_ov006_0213cb8c;

        obj = c + 0x5024;
        p = *(void **)((char *)obj + 4);
        func_02045e44(obj, 1, 0);
        *(int *)((char *)p + 0x18) = *(int *)(c + 0x500c);

        {
            void *obj2 = (void *)(c + 0x501c);
            ((void (**)(void *, void *))(*(int *)obj2))[5](obj2, &t);
        }
    }

    func_ov006_020c70d0();
    func_ov006_020c425c();

    return 1;
}
