/* HOST COPY of src/func_ov006_02127d10.c -- dScMgSnowball_c's Render, vtable
 * slot 9.  Run mg11, lane SNW.
 *
 * ---- WHAT IT FIXES: THE LOCAL SHADOW CLASS, CAUGHT BY A RUN --------------
 *
 * This is the third instance in the port of the defect
 * port/unmatched/Door_Render.cpp and port/unmatched/MgFlower_ModelRender_
 * 020c3bf4.cpp already name, and the first one this lane had no static
 * detector for: a matched src TU that dispatches a model virtual through a
 * LOCAL FUNCTION-POINTER read rather than through a typed class.
 *
 * src/func_ov006_02127d10.c, verbatim:
 *
 *     void *self = (void*)(c + 0xaba4);
 *     void (*fn)(void*, int*) =
 *         *(void(**)(void*, int*))((char*)(*(void**)self) + 0x14);
 *     fn(self, vecArr);
 *
 * and the ROM it is transcribed from, read off extracted/overlays/
 * overlay_0006.bin at base 0x020bfec0:
 *
 *     02127e48  ldr  r0,[pc,#0x51c]     ; pool 0x0212836c = 0x0000aba4
 *     02127e4c  add  r1,sp,#0x18        ; &vecArr
 *     02127e50  add  r0,sb,r0           ; the Model at this+0xaba4
 *     02127e54  ldr  r2,[r0]            ; its vptr
 *     02127e58  ldr  r2,[r2,#0x14]      ; BYTE +0x14 = the sixth Itanium word
 *     02127e5c  blx  r2
 *
 * The object at +0xaba4 is a plain Model: func_ov006_021295ac (the
 * constructor) builds it with _ZN5ModelC1Ev(t + 0xaba4) and
 * func_ov006_02129268 (slot 0, InitResources) hands it a file through
 * ModelBase::SetFile(this + 0xaba4, ...).
 *
 * THE VTABLE READ IS NOT THE PROBLEM AND THAT WAS MEASURED, not assumed.
 * hal/cxxname_bridge.cpp DUAL-FILLS _ZTV5Model at both [4] and [5] with
 * mv_render for exactly this shape, and a probe in this seat's render thunk
 * printed the live values on the first Render of scene 377:
 *
 *     [snw-probe] sub +0xaba4 vptr=00CBD6B0 aba0=16384
 *     [snw-probe]   word 0 = 00426F90     word 4 = 004281A0   mv_render
 *     [snw-probe]   word 1 = 00428150     word 5 = 004281A0   mv_render
 *     [snw-probe]   word 2 = 00428390     word 6 = 00000000
 *     [snw-probe]   word 3 = 004283A0     word 7 = 00000000
 *
 * 0x00CBD6B0 is __ZTV5Model in walk_window.map and 0x004281A0 is
 * ?mv_render@@YIXPAX0PBX@Z.  So byte +0x14 resolves to the RIGHT FUNCTION.
 *
 * THE PROBLEM IS THE CALLING CONVENTION, WHICH IS THE mg10 INVISIBLE-DEFECT
 * CLASS: a C function-pointer indirect call into a __fastcall thunk.  Both
 * checkers are blind to it -- there is no unresolved symbol and no arity row,
 * because the source type IS a function pointer and it IS called with the
 * arguments it declares.
 *
 *   mv_render is  void __fastcall (void *self, void *unused, const void *s)
 *   the src's fn is  void __cdecl  (void *self, int *s)
 *
 * Under __fastcall the receiver arrives in ecx, the dummy in edx and `s` on
 * the stack, and the callee cleans four bytes on the way out.  Under the
 * src's cdecl both arguments go on the stack and the CALLER cleans eight.  So
 * mv_render read a stale ecx as its receiver and the model pointer as its
 * scale vector, and the stack came back four bytes short.
 *
 * MEASURED, scene 377, SM64DS_FAULTS_FATAL=1, this tree, before this file:
 *
 *     FAULT code c0000005 at +0x303f3a64 accessing 307f3a64
 *       walker node 307F3A9C actor 307F3A64 id 0x179
 *       eip 307f3a64  eax 307f3a64  ecx 307f3a64  edi 307f3a64
 *
 * eip IS the actor's own address: the double-cleaned stack put the object
 * pointer where a return address belonged and the frame returned into it.
 * That is the same signature run mg9 lane BOO recorded for the framework's
 * slot-19 shadow (ebp = port_mg_try_base_state+0x28b), one class over.
 *
 * The A/B that isolates it needs no patch: SM64DS_SCENE_SLOT9=0 leaves the
 * Render slot on a no-op and the same 300-frame run exits 0, and
 * SM64DS_SCENE_NO_RENDER=1 likewise.  Everything else about the scene --
 * 27308 routed state dispatches over 300 frames, all fourteen table slots
 * entered, zero unhandled addresses -- is unchanged by this file.
 *
 * ---- THE CALL IS SPELLED AS THE METHOD THE ROM MEANS ---------------------
 *
 * `((Model *)mdl)->Render((const Vector3 *)vecArr)` -- a VIRTUAL call through
 * the C++ type, so MSVC forms the receiver, the convention and the slot
 * itself.  It stays virtual rather than qualified for the reason
 * MgFlower_ModelRender_020c3bf4.cpp gives, and it is safe to type it as Model
 * for the reason Door_Render.cpp gives: this object is exactly a Model by
 * construction, built by _ZN5ModelC1Ev in this class's own constructor.
 *
 * The second reading that says slot 5 is Render here is the ARGUMENT.  The
 * three words handed over are the same value three times --
 * `v / 2 + v * 4`, from the field at +0xaba0 -- which is a SCALE VECTOR, and
 * Render(const Vector3 *scale) is the only virtual on either model class that
 * takes one.  Virtual10 takes a matrix by reference and Virtual18 takes a
 * matrix and a scale; neither reading survives a uniform triple.
 *
 * WHAT CHANGED FROM src, so it can be checked line by line: the local `fn`
 * read and its call are replaced by that method call, and the includes and
 * externs are re-spelled for C++.  Every other statement, constant, offset,
 * loop bound and branch is the matched source's, transcribed unchanged --
 * including the raw store of the packed light vector to 0x040004c8, both
 * 0x80-iteration passes with their +0xac58/+0xb358 gates, the 0x258/0x276
 * timer bands and the two GetGameLanguage lookups into data_ov006_0213fff0.
 */

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (MSVC thiscall vs the slice's cdecl).

#include "types.h"
#include "Model.h"
#include "dScMgSnowball_c.h"

extern "C" {
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void func_0203cd80(int *m, short angle);
extern void func_ov006_02126a98(char *c);
extern void func_ov004_020b2220(int a1, int a2, int a3, int a4, int a5, int a6,
                                int a7);
extern int  GetGameLanguage(void);
extern int  func_ov004_020b04c0(void);
extern void func_ov004_020b2444(int, int, int, int, int, int, int);
extern void func_ov004_020b0380(void *, int, int, void *);
extern void func_ov006_02129690(void *);
extern void func_ov006_02125804(char *);

extern void *data_ov006_02139d24[2];
extern void *data_ov006_02138d64[];
extern void *data_ov006_02139c24;
extern void *data_ov006_02139c80[];
extern void *data_ov006_02139d18;
extern void *data_ov006_02139c38;
extern void *data_ov006_02139c38;
extern void *data_ov006_0213fff0[];
}

extern "C" int func_ov006_02127d10(char *c)
{
    struct dScMgSnowball_c *self = (struct dScMgSnowball_c *)(void *)c;
    int m[3];
    int vecArr[3];

    *(int *)(c + 0xb9d8) = *(int *)(c + 0xb9d8) + 1;
    if (self->unk_b9d8 >= 0x20) {
        self->unk_b9d8 = 0;
    }

    {
        int base = self->unk_ba04;
        int t = (((self->unk_ab3c >> 0xc) - base) * 0x7c) / (self->unk_ba00 - base);
        func_ov004_020afdd0(data_ov006_02139d24[0], 0xf0, t + 0x22, -1, 0);
    }
    func_ov004_020afdd0(data_ov006_02139d24[1], 0xf0, 0x60, -1, 0);

    m[2] = 0xfffff008;
    m[1] = 0;
    m[0] = 0;
    func_0203cd80(m, -0x2000);
    *(volatile int *)0x040004c8 =
        (((short)m[0] >> 3) & 0x3ff) |
        ((((short)m[1] >> 3) & 0x3ff) << 10) |
        ((((short)m[2] >> 3) & 0x3ff) << 20);
    *(int *)0x040004cc = 0x7fff;

    {
        int v = self->unk_aba0;
        int t = v / 2 + v * 4;
        vecArr[0] = t;
        vecArr[1] = t;
        vecArr[2] = t;
        if (self->unk_aba0 > 0) {
            /* THE ONE CHANGED LINE PAIR. src reads byte +0x14 of the vptr into
               a cdecl function pointer and calls it; that is the ROM's own
               dispatch and the port cannot make the call. */
            Model *mdl = (Model *)(void *)(c + 0xaba4);
            mdl->Render((const Vector3 *)vecArr);
        }
    }

    func_ov006_02126a98(c);

    for (int i1 = 0; i1 < 0x80; i1++) {
        char *p = c + i1;
        if (*(u8 *)(p + 0xac58) == 1) {
            int t6 = self->unk_ab6c;
            int t5 = *(int *)(c + i1 * 8 + 0xacdc);
            if (t5 >= t6 - 0x20000 && t5 < t6 + 0x1a0000 + (func_ov004_020b04c0() << 0xc)) {
                if (*(int *)(c + i1 * 4 + 0xb0d8) == 1) {
                    int cnt = self->unk_b9d8;
                    int idx = (cnt / 4) & 7;
                    if (*(u8 *)(p + 0xb2d8) == 1) idx += 8;
                    int a1 = (*(int *)(c + i1 * 8 + 0xacd8) - self->unk_ab68) >> 0xc;
                    int a2 = ((t5 - t6) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02138d64[idx], a1, a2, -1, -1);
                } else {
                    int a1 = (*(int *)(c + i1 * 8 + 0xacd8) - self->unk_ab68) >> 0xc;
                    int a2 = ((t5 - t6) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02139c24, a1, a2, -1, 2);
                }
            }
        }
    }

    for (int i2 = 0; i2 < 0x80; i2++) {
        if (*(u8 *)(c + i2 + 0xb358) == 1) {
            int t7 = self->unk_ab6c;
            int t6 = *(int *)(c + i2 * 8 + 0xb5dc);
            if (t6 >= t7 - 0x40000 && t6 < t7 + 0x1c0000 + (func_ov004_020b04c0() << 0xc)) {
                switch (*(int *)(c + i2 * 4 + 0xb3d8)) {
                case 0:
                case 1:
                case 2: {
                    int a1 = (*(int *)(c + i2 * 8 + 0xb5d8) - self->unk_ab68) >> 0xc;
                    int a2 = ((t6 - t7) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02139d18, a1, a2, -1, 1);
                    break;
                }
                case 3: {
                    int v = *(int *)(c + i2 * 8 + 0xb5d8);
                    int sel = (v < 0x80000) ? 1 : 0;
                    int a1 = (v - self->unk_ab68) >> 0xc;
                    int a2 = ((t6 - t7) >> 0xc) - 0x110;
                    func_ov004_020afdd0(data_ov006_02139c80[sel], a1, a2, -1, 1);
                    break;
                }
                }
            }
        }
    }

    {
        int a1 = (self->unk_ab48 - self->unk_ab68) >> 0xc;
        int a2 = ((self->unk_ab4c - self->unk_ab6c) >> 0xc) - 0x110;
        func_ov004_020afdd0(data_ov006_02139c38, a1, a2, -1, 2);
    }

    {
        int t4 = self->unk_b9fc;
        if (t4 <= 0xf0 && t4 > 0x3c) {
            int q = t4 / 60;
            if (q >= 4) q = 3;
            func_ov004_020b2220(0x80, 0x60, q, -1, -1, 0x800, 0);
        } else {
            int mode = self->unk_b9f4;
            if (mode == 1 || mode == 2
                || (mode == 3 && self->unk_ba10 > 0)
                || (mode == 4 && self->unk_ba0c <= 0x3c)) {
                int v = self->unk_b9dc;
                int q60 = v / 60;
                int rem60 = v % 60;
                int r7 = rem60 * 100;
                int centis = r7 / 60;
                int r4;
                int tens;
                int ones;
                if (v < 0x258) {
                    r4 = 0xc;
                } else if (v >= 0x276) {
                    r4 = -0x104;
                } else {
                    r4 = ((0x258 - v) * 0x110) / 30 + 0xc;
                }
                func_ov004_020b2444(0x84, r4, q60, -1, -1, 1, 0);
                tens = centis / 10;
                func_ov004_020b2444(0x9c, r4, tens, -1, -1, 1, 0);
                ones = centis % 10;
                if (ones == 5 || ones == 0) {
                    func_ov004_020b2444(0xac, r4, ones, -1, -1, 1, 0);
                } else {
                    func_ov004_020b2444(0xac, r4, ones + 1, -1, -1, 1, 0);
                }
                {
                    int idx = GetGameLanguage();
                    func_ov004_020b0380(*(void **)((char *)data_ov006_0213fff0[idx] + 0xc),
                                        0x5c, r4, 0);
                }
                {
                    int idx = GetGameLanguage();
                    func_ov004_020b0380(*(void **)((char *)data_ov006_0213fff0[idx] + 0x1c),
                                        0x90, r4, 0);
                }
            }
        }
    }

    func_ov006_02129690(c);
    func_ov006_02125804(c);
    return 1;
}
