//cpp
// @symbol _ZN12dScMgJump2_c6RenderEv
#include "types.h"
#include "decl_common.h"
#include "dScMgJump2_c.h"
/* dScMgJump2_c::Render -- vtable slot 9, ov006 0x020ef148.
 *
 * Attributed by the vtable: dScMgJump2_c's own table is ov006 0x0213ccfc and
 * its slot 9 relocates here. The signature is include/dScMgBase_c.h's own slot
 * 9, `virtual s32 Render()`.
 *
 * THE TWO STRUCT COPIES ARE THE ONE THING IN THIS FILE THAT IS NOT FREE, and
 * S3 HAD TO CHANGE SHAPE TO SURVIVE THE MOVE TO //cpp. A whole-struct
 * assignment is a memcpy in C and is scalarised in C++ (notes/
 * mwccarm-codegen.md), and the pre-migration file's `struct S3 { int a, b, c; }`
 * duly came out three ldr/str pairs where the ROM has one ldm/stm -- 0x17c
 * against 0x170, twelve bytes, exactly the defect that note names. Declaring
 * the same three words as an ARRAY member restores the block copy. It is a
 * size threshold and not a language-mode absolute: M48, twelve words, block-
 * copies in C++ either way, which is why the 0x5a30 matrix store needed
 * nothing. Both stay anonymous word arrays rather than named matrix or vector
 * types -- spelling either as the real type is a separate change that has to
 * be re-measured against this same gate.
 *
 * M48 is the 12-word Matrix4x3 the ROM writes into mModel at +0x1c; S3 is the
 * three-word argument the model's own slot-5 virtual takes.
 *
 * mModel is raw bytes here for the reason include/dScMgJump2_c.h's banner
 * gives -- the ROM's destructor destroys it before the three arrays, which a
 * typed member could never reproduce -- so its insides are reached by offset:
 * +0x00 is the vptr the last call dispatches through, +0x08 the object
 * func_02045e44 steps, +0x1c the matrix. */

extern "C" {
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
int GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern int data_020a0e68;
}

struct M48 { int w[12]; };
struct S3 { int w[3]; };

s32 dScMgJump2_c::Render()
{
    char *self = (char *)this;
    struct S3 local;
    void *p;
    int count;
    int a1v;
    int i;

    func_ov004_020b1a5c(data_ov006_02140308, 4);
    func_ov006_020eef58();
    func_ov006_020ef2b8();
    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0xffead000, -0xa0000);
    *(struct M48 *)(self + 0x5a30) = *(struct M48 *)&data_020a0e68;

    local = *(struct S3 *)&data_ov006_0213ccb0;

    {
        int *obj = (int *)(self + 0x5a1c);
        p = (void *)obj[1];
        func_02045e44((void *)obj, 1, 0);
    }
    *(int *)((char *)p + 0x18) = unk_5a64;
    {
        void **vobj = (void **)(self + 0x5a14);
        (*(void (**)(void *, void *))((char *)*vobj + 0x14))((void *)vobj, &local);
    }

    func_ov006_020c70d0();
    func_ov006_020c425c();

    if (*(u16 *)(self + 0x4664) == 1) {
        a1v = 0x6e;
        count = data_ov006_02140428;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)data_ov006_0213cccc[idx][1], (void *)a1v, 0xc, (void *)0);
            } else {
                RenderOamMainScreen(data_ov006_02134cf8, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    return 1;
}
