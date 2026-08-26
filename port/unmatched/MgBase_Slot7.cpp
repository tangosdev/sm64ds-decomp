/* HOST COPY of dScMgBase_c slot 7, func_ov004_020b0620. Run link60, lane MG1.
 *
 * THE ONLY ONE OF THE FOUR THAT CHANGES BEHAVIOUR, and it changes it toward
 * the ROM rather than away from it. See the block above the body.
 *
 * port/unmatched/MgBase_DeclConflict.cpp carries the shared derivation of the
 * declaration conflict that puts all four bodies in port/unmatched/.
 *
 * WHAT CHANGED: `char *self` became `void *cv`, plus a first line
 * `char *self = (char *)cv;`, plus ONE ARGUMENT at the Scene::BeforeBehavior
 * call. Nothing else.
 *
 * decl_Scene.h IS DELIBERATELY NOT INCLUDED HERE. That header is the whole
 * reason this body needs repairing -- it declares Scene::BeforeBehavior with
 * empty parens -- and including it would make the corrected one-argument call
 * a hard error instead of a fix. The one declaration this TU needs is written
 * out below with the signature the real definition has.
 *
 * Ruled REAL_DECOMP against the ROM before seating: 0x220, 129 instructions,
 * the largest of the framework's twenty. See
 * port/tools/inferred_stub_adjudicated.txt.
 */
#include "types.h"
#include "decl_common.h"

/* src/_ZN5Scene14BeforeBehaviorEv.cpp's real signature, which
   include/decl_Scene.h does not carry. */
extern "C" int _ZN5Scene14BeforeBehaviorEv(char *self);

/* ---- func_ov004_020b0620, slot 7, BeforeBehavior ------------------------- */
/*
 * THIS COPY REPAIRS A SECOND DEFECT AND IT IS NOT THE DECLARATION CONFLICT.
 * The src body's first statement is
 *
 *     if (_ZN5Scene14BeforeBehaviorEv() == 0)
 *
 * with NO ARGUMENT, because include/decl_Scene.h declares
 *
 *     extern int _ZN5Scene14BeforeBehaviorEv();
 *
 * with empty parens inside extern "C". The real definition,
 * src/_ZN5Scene14BeforeBehaviorEv.cpp, is
 *
 *     int _ZN5Scene14BeforeBehaviorEv(char* self)
 *
 * and it forwards self straight into ActorBase::BeforeBehavior, which
 * dereferences it. On ARM the call is CORRECT: r0 already holds self on entry
 * to this function and mwccarm emits nothing, so the ROM bytes and the src
 * agree and the byte gate is right to be silent. On the host it is a dropped
 * argument -- MSVC's __cdecl pushes nothing and the callee reads the stack --
 * and the very first thing every minigame frame does is dispatch this slot.
 *
 * TWO LINES BELOW IT THE SAME HEADER SPELLS THE SIBLING CORRECTLY,
 * `_ZN5Scene19BeforeInitResourcesEv(void*)`, which is what makes this a defect
 * rather than a convention -- and slot 1 above, which the same lane copied for
 * the unrelated reason, calls that sibling WITH its argument.
 *
 * So this copy passes `self`. That is the ONE semantic change in this file;
 * everything else in all four bodies is a signature spelling. The header fix
 * proper belongs in include/decl_Scene.h and is routed rather than taken,
 * because changing a declaration that thousands of TUs see is not a port
 * lane's call to make from inside port/.
 */
extern "C" {
int func_ov004_020b8ee0(char *p);
void func_ov004_020aeb24(char *c);
void func_ov004_020b8778(char *c);
void func_ov004_020b321c(char *c);
void func_ov004_020adf2c(char *c);
void _Z14ApproachLinearRiii(int *p, int a, int b);
}

namespace mg20 {
struct Scene {
    virtual int f00(); virtual int f01(); virtual int f02();
    virtual int f03(); virtual int f04(); virtual int f05();
};
struct Self {
    virtual void v00();  virtual void v01();  virtual void v02();  virtual void v03();
    virtual void v04();  virtual void v05();  virtual void v06();  virtual void v07();
    virtual void v08();  virtual void v09();  virtual void v10();  virtual void v11();
    virtual void v12();  virtual void v13();  virtual void v14();  virtual void v15();
    virtual void v16();  virtual void v17();  virtual void v18();  virtual void v19();
    virtual void v20();  virtual void v21();  virtual void v22();  virtual void v23();
    virtual void v24();  virtual void v25();  virtual void v26();
    virtual void m6c();  /* index 27 = 0x6c */
    virtual void m70();  /* index 28 = 0x70 */
};
}  /* namespace mg20 */

/* ---- lane RESULTS (mg14): four counters, host bookkeeping ----------------
 *
 * Same class as the MG_SLOT counters in hal/scene_mg.cpp and for the same
 * reason: from outside, a slot that is never dispatched and a slot that runs
 * and takes an early return look identical, and only a counter tells them
 * apart. This body is the ONE place both questions can be asked at once --
 * it is the caller of the results panel's tick and the caller of
 * Scene::BeforeBehavior that can veto it -- so the four live here rather than
 * in a fifth file. They cost three increments a frame and are read by
 * port_mg_results_watch, which prints nothing unless SM64DS_MG_RESULTS_PROBE
 * is set.
 *
 * They earned their place: mg12 read the panel's frozen record and concluded
 * the FILL was missing, when the fill was correct and the tick was returning
 * early. slot7 vs tick_in vs tick_out separates "never called", "called and
 * vetoed", "called and early-returned" and "called and faulted" in one line. */
extern "C" {
extern unsigned g_res_slot7, g_res_tick_in, g_res_tick_out, g_res_bb_zero;
}

extern "C" {
extern mg20::Scene *data_0209f5bc;
extern unsigned char data_020a0e40;
extern unsigned short data_020a0e5a[];
extern unsigned char data_020a0de8[];
extern int data_0208ee44;

int func_ov004_020b0620(void *cv)
{
    char *self = (char *)cv;                    /* the one added line */
    int mode;
    unsigned short flags;

    ++g_res_slot7;
    /* the ONE semantic change: self is passed. See the block above. */
    if (_ZN5Scene14BeforeBehaviorEv(self) == 0) {
        ++g_res_bb_zero;
        return 0;
    }

    if (((mg20::Scene *)data_0209f5bc)->f05()) {
        mode = data_020a0e40;
        flags = data_020a0e5a[mode * 2];
        if ((flags & 8) || (flags & 4) || (flags & 1) || (flags & 2)) {
            if (*(int *)(self + 0x4628) != 0) {
                if (*(short *)(self + 0x4646) < 0) {
                    if (func_ov004_020b8ee0(self + 0xf4))
                        ((mg20::Self *)self)->m70();
                }
            } else {
                if (data_020a0de8[mode * 4] == 0)
                    ((mg20::Self *)self)->m6c();
            }
        }
    }

    if (*(int *)(self + 0x4628) != 0) {
        ++g_res_tick_in;
        func_ov004_020aeb24(self);
        ++g_res_tick_out;
        return 0;
    }
    if (*(int *)(self + 0x462c) != 0)
        return 0;

    if (*(int *)(self + 0xf0) == 0) {
        int i;
        char *g;
        func_ov004_020b8778(self + 0xcc);
        g = data_ov004_020bf648;
        for (i = 0; i < 3; i++) {
            if (*(int *)(g + 0x20) != 0x1d)
                func_ov004_020b321c(g);
            g += 0x134;
        }
    }

    {
        int j;
        char *p = data_ov004_020bebe8;
        char *base = data_ov004_020bebe8;
        for (j = 0; j < 0x40; j++) {
            if (*(short *)(base + j * 0x20 + 0x1a) != 0)
                func_ov004_020adf2c(p);
            p += 0x20;
        }
    }

    if (*(int *)(self + 0xa4) == 0) {
        if (((mg20::Scene *)data_0209f5bc)->f05() == 0)
            return 0;
    }

    _Z14ApproachLinearRiii((int *)(self + 0xac), *(int *)(self + 0xa8),
                           data_0208ee44);

    {
        int *p = (int *)(((int)self + 0x5c));
        *p = *p + 1;
    }
    if (*(int *)(self + 0x5c) >= 0x28)
        *(int *)(self + 0x5c) = 0;

    return 1;
}
}  /* extern "C" */
