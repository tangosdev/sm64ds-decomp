/* ov073's POINTER-TO-MEMBER STATE MACHINES -- the twenty runtime dispatch
 * tables Chief Chilly and the Ccc arena run on, seated with host bodies, plus
 * the four bodies of theirs whose matched src cannot be called as written.
 *
 * run linkw wave 12, lane w12. Companion to hal/actor_classes_ov073.cpp; the
 * mount and the width derivations are in port/ov073_syms.txt.
 *
 * ==== THE TABLES, READ OUT OF THE OVERLAY IMAGE ============================
 *
 * Forty 8-byte {code, adj} source statics sit in ov073's .data and the two
 * sinits copy them into twenty bss tables of two records each:
 *
 *   source run                   records  copied by                into
 *   0x02122f40..0x02123040          32    __sinit_ov073_02122874   sixteen
 *                                                                  tables,
 *                                                    0x02123320..0x02123420
 *   0x0212310c..0x0212314c           8    __sinit_ov073_02122d48   four tables,
 *                                    0x021234a0/0x021234b0/c0/d0
 *
 * Every one of the forty adj words is a ROM ZERO (the non-virtual
 * complete-class Itanium form) -- read from extracted/overlays/overlay_0073.bin
 * and cross-checked word for word against config/arm9/overlays/ov073/relocs.txt.
 * The forty code words are DS addresses the mount's pointer pass cannot rebase
 * (they point into .text, which is outside a packed mount's data window), so
 * port_ov073_states_seat below rewrites each one with its host body, VERIFYING
 * the stored word against the ROM address first. It runs BEFORE both sinits
 * (hal/actor_classes_ov073.cpp's bring-up), the ov060/Crate ordering: the
 * sinits COPY these records, so a seat that ran after them would fix the
 * sources and leave every live table holding DS addresses.
 *
 * ==== WHY FOUR BODIES ARE HOST COPIES ======================================
 *
 * (1) ChiefChilly_ChangeState and func_ov073_021223a4 (CccArena's ChangeState)
 *     are the two dispatchers, and both spell a REAL MSVC pointer-to-member:
 *
 *         struct C; typedef int (C::*PMF)(); struct C { char pad[N]; PMF *pp; };
 *
 *     which is the ov060 lane's measured "forward-declared C, DEFINED later in
 *     the same TU" shape -- SIXTEEN bytes, against the ROM's eight. On top of
 *     the stride, MSVC dispatches a pointer-to-member as __thiscall (receiver
 *     in ecx) while every ov073 state body is a cdecl `f(char *)`, so even a
 *     correctly strided read would hand the body a garbage argument. Both
 *     faults are the Crate / Chuckya / MontyMole / ov060 case and the fix is
 *     the same: read the record as a plain {function, adj} pair and call the
 *     function with `this`. The decode below is the ROM's own, transcribed from
 *     0x021223a4 and 0x0212157c, which are instruction-for-instruction
 *     identical apart from the member offset (0x320 / 0x37c):
 *
 *         ldr r3, [r0, #OFF]      ; pp
 *         ldr r2, [r3]            ; fn
 *         cmp r2, #0 -> return 1
 *         ldr r1, [r3, #4]        ; adj
 *         add r0, r0, r1, asr #1  ; receiver = this + (adj >> 1)
 *         ands r1, r1, #1
 *         ldrne r1, [r0] / ldrne r1, [r1, r2]   ; virtual: *(*recv + fn)
 *         ldreq r1, [r3]                        ; nonvirtual
 *         blx r1
 *
 *     _ZN8CccArena8BehaviorEv does the same decode BY HAND in matched src, so
 *     it is right as written and stays in the slice. It is the model this
 *     helper follows.
 *
 * (2) _ZN11ChiefChilly8BehaviorEv is the matched TU line for line with THREE
 *     lines changed: it holds one inline copy of the same dispatch
 *     (`PMF *p = c->pp + 1; (c->**p)();`, which under MSVC steps 16 bytes
 *     instead of 8 and calls __thiscall). Everything else in it -- the
 *     fourteen state-pointer comparisons, the raycast, the dust, the collider
 *     work -- is unchanged, and the src TU is byte-locked in src/ as the
 *     record of what it was. The ROM's own dispatch at 0x021218c0 reads
 *     `r1 = this->pp; r0 = r1[8]` and then `r3 = r1 + 8`, which is the +8 this
 *     copy spells as `c->pp + 2`.
 *
 * (3) _ZN11ChiefChilly6RenderEv is the BlendModelAnim slot-5 collision and it
 *     is in port/unmatched/Ov073_Render.cpp, not here: it needs the port's real
 *     BlendModelAnim.h, and so the port's Vector3, while the transplanted
 *     Behavior below declares its own `struct Vector3 { int x, y, z; };` at
 *     file scope. Rather than reword a body that is otherwise its source
 *     verbatim, the two live in separate translation units.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

/* THE ROM'S OWN POINTER-TO-MEMBER DECODE, from 0x021223a4 / 0x0212157c.
   `rec` is a {function, adj} pair; adj is zero in all forty ov073 records, but
   the full decode is transcribed rather than the zero case, because a record
   the seat did not write would otherwise take a silently different path. */
static int ov73_pmf_call(char *self, int *rec)
{
    int fn = rec[0];
    int adj = rec[1];
    char *recv = self + (adj >> 1);
    if (adj & 1) {
        char *vt = *(char **)recv;
        fn = *(int *)(vt + fn);
    }
    return ((int (*)(void *))fn)(recv);
}

/* ---- the two dispatchers -------------------------------------------------- */

/* func_ov073_021223a4 IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov073_021223a4.cpp back on port/slice_pmf2.txt: with /vmg /vmm global (the
   R8 block in port/CMakeLists.txt) MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the matched TU compiles to the same
   tail jump this body was -- measured, listing in that slice's header.
   The reading above is kept because it is the derivation. */

/* ChiefChilly_ChangeState IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/ChiefChilly_ChangeState.cpp back on port/slice_pmf2.txt: with /vmg /vmm global (the
   R8 block in port/CMakeLists.txt) MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the matched TU compiles to the same
   tail jump this body was -- measured, listing in that slice's header.
   The reading above is kept because it is the derivation. */
}  /* extern "C" */

/* ---- ChiefChilly::Behavior, the matched TU with the dispatch corrected ----
   Everything below this line is src/_ZN11ChiefChilly8BehaviorEv.cpp verbatim
   except the three lines named in note (2) of this file's header. */
struct Vector3 { int x, y, z; };
struct Mat4x3 { int m[12]; };

struct C { char pad[0x37c]; int *pp; };   /* ROM: a pointer-to-member array */

struct RayParams { Vector3 start, end, in, out; };

extern "C" {
extern void *data_0209f318;
extern Mat4x3 data_020a0e68;
extern Vector3 data_ov073_02123040;
extern char data_ov073_02123320[];
extern char data_ov073_02123330[];
extern char data_ov073_02123340[];
extern char data_ov073_02123350[];
extern char data_ov073_02123360[];
extern char data_ov073_02123380[];
extern char data_ov073_02123390[];
extern char data_ov073_021233a0[];
extern char data_ov073_021233c0[];
extern char data_ov073_021233d0[];
extern char data_ov073_021233f0[];
extern char data_ov073_02123400[];

extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *clsn);
extern int _ZNK9Animation12WillHitFrameEi(void *self, int f);
extern void MulMat4x3Mat4x3(void *d, void *a, void *b);
extern void Vec3_Lsl(Vector3 *d, Vector3 *s, int sh);
extern void func_02012694(int a, void *b);
extern void _ZN5Actor17HugeLandingDustAtER7Vector3b(void *self, Vector3 *v, int b);
extern void _ZN11RaycastLineC1Ev(void *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
extern void MulVec3Mat4x3(void *a, void *m, void *out);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, const Vector3 *a, const Vector3 *b, void *actor);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wmc, unsigned int flags);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
extern void func_ov073_0211f61c(void *self);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void func_ov073_021215cc(void *self);
extern void _ZN14BlendModelAnim7AdvanceEv(void *self);
}

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch: the matched TU holds an
 * inline `PMF *p = c->pp + 1; (c->**p)();` that steps 8 bytes on ARM and 16
 * under MSVC. Three lines changed, the rest is the matched body verbatim. See
 * note (2) in this file's header. */
extern "C" int _ZN11ChiefChilly8BehaviorEv(C *c)
{
    char *self = (char *)c;
    int angx;
    Vector3 v0;
    RayParams rp;
    Vector3 v3C;
    Vector3 v48;
    Vector3 v54;
    int line[0x1f];

    *(C **)((char *)data_0209f318 + 0x114) = c;
    DecIfAbove0_Short((unsigned short *)(self + 0x100));

    if (*(void **)((char *)c->pp + 8) != 0)
        ov73_pmf_call(self, c->pp + 2);   /* record[1]: ROM steps pp by 8 bytes */

    if ((char *)c->pp != data_ov073_02123400
        && (char *)c->pp != data_ov073_02123320
        && (char *)c->pp != data_ov073_02123340) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(self, self + 0x110);
    } else {
        int sum = *(int *)(self + 0xa8) + *(int *)(self + 0x9c);
        int m = *(int *)(self + 0xa0);
        int ac = *(int *)(self + 0xac);
        if (sum >= m) m = sum;
        *(int *)(self + 0xa8) = m;
        *(int *)(self + 0xac) = ac;
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(self, self + 0x110);
    }

    if (((char *)c->pp == data_ov073_02123330 || (char *)c->pp == data_ov073_02123350)
        && _ZNK9Animation12WillHitFrameEi(self + 0x35c, 7) != 0) {
        data_020a0e68 = *(Mat4x3 *)(self + 0x328);
        MulMat4x3Mat4x3(*(char **)(self + 0x320) + 0x60, &data_020a0e68, &data_020a0e68);
        v0.x = data_020a0e68.m[9];
        v0.y = data_020a0e68.m[10];
        v0.z = data_020a0e68.m[11];
        Vec3_Lsl(&v3C, &v0, 3);
        v0 = v3C;
        func_02012694(0x167, self + 0x74);
        v48 = v0;
        _ZN5Actor17HugeLandingDustAtER7Vector3b(self, &v48, 1);
    }

    if ((char *)c->pp != data_ov073_021233c0
        && (char *)c->pp != data_ov073_021233d0
        && (char *)c->pp != data_ov073_021233f0
        && (char *)c->pp != data_ov073_02123400
        && (char *)c->pp != data_ov073_02123320
        && (char *)c->pp != data_ov073_02123340
        && (char *)c->pp != data_ov073_02123380) {
        _ZN11RaycastLineC1Ev(line);
        rp.start.x = 0; rp.start.y = 0; rp.start.z = 0;
        rp.end.x = 0; rp.end.y = 0; rp.end.z = 0;
        rp.in.x = 0; rp.in.y = 0; rp.in.z = 0;
        rp.out.x = 0; rp.out.y = 0; rp.out.z = 0;
        {
            int y;
            rp.start.x = *(int *)(self + 0x5c);
            angx = 0x2000;
            y = *(int *)(self + 0x60);
            rp.start.y = y;
            rp.start.z = *(int *)(self + 0x64);
            rp.start.y = y + 0x78000;
            if (*(unsigned char *)(self + 0x4cb) > 1)
                rp.in.z = 0x258000;
            else
                rp.in.z = 0x12c000;
            Matrix4x3_FromRotationY(&data_020a0e68, *(short *)(self + 0x94));
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, angx);
        }
        MulVec3Mat4x3(&rp.in, &data_020a0e68, &rp.out);
        {
            int sx = rp.start.x;
            int ox = rp.out.x;
            int sy = rp.start.y;
            int sz = rp.start.z;
            int oy, oz;
            rp.end.x = sx;
            rp.end.x = sx + ox;
            oy = rp.out.y;
            oz = rp.out.z;
            rp.end.y = sy;
            rp.end.y = sy + oy;
            rp.end.z = sz;
            rp.end.z = sz + oz;
        }
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(line, &rp.start, &rp.end, self);
        if (_ZN11RaycastLine10DetectClsnEv(line) == 0) {
            if (*(int *)(self + 0x98) > 0xa000) {
                *(unsigned char *)(self + 0x4c9) = 1;
            }
            *(int *)(self + 0x4ec) = *(int *)(self + 0x5c);
            *(int *)(self + 0x4f0) = *(int *)(self + 0x60);
            *(int *)(self + 0x4f4) = *(int *)(self + 0x64);
            if ((char *)c->pp != data_ov073_021233a0) {
                *(int *)(self + 0x5c) = *(int *)(self + 0x68);
                *(int *)(self + 0x60) = *(int *)(self + 0x6c);
                *(int *)(self + 0x64) = *(int *)(self + 0x70);
                *(int *)(self + 0x98) = 0;
            }
        } else {
            *(unsigned char *)(self + 0x4c9) = 0;
        }
        _ZN11RaycastLineD1Ev(line);
    }

    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(self, self + 0x150, 0);

    v54 = data_ov073_02123040;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(self + 0x110, &v54);

    if ((char *)c->pp == data_ov073_02123360
        || (char *)c->pp == data_ov073_02123390) {
        func_ov073_0211f61c(self);
    }
    _ZN12CylinderClsn5ClearEv(self + 0x110);
    _ZN12CylinderClsn6UpdateEv(self + 0x110);
    func_ov073_021215cc(self);
    _ZN14BlendModelAnim7AdvanceEv(self + 0x30c);
    return 1;
}

/* ---- the seat ------------------------------------------------------------ */
extern "C" {
/* the forty state bodies, all matched src carried by port/slice_w12.txt.
   Declared only so their host addresses can be taken; each is a cdecl
   `int f(<one pointer>)` in its own TU and the seat stores the address. */
void func_ov073_0211fa74(void);
void func_ov073_0211fbec(void);
void func_ov073_0211fbf4(void);
void func_ov073_0211fc70(void);
void func_ov073_0211fc78(void);
void func_ov073_0211fe84(void);
void func_ov073_0211fe8c(void);
void func_ov073_0212000c(void);
void func_ov073_0212005c(void);
void func_ov073_02120098(void);
void func_ov073_021200e0(void);
void func_ov073_02120390(void);
void func_ov073_021203ac(void);
void func_ov073_021205f0(void);
void func_ov073_02120610(void);
void func_ov073_0212081c(void);
void func_ov073_02120844(void);
void func_ov073_021208e4(void);
void func_ov073_02120910(void);
void func_ov073_02120ad8(void);
void func_ov073_02120b78(void);
void func_ov073_02120c08(void);
void func_ov073_02120c7c(void);
void func_ov073_02120d80(void);
void func_ov073_02120dec(void);
void func_ov073_02120e60(void);
void func_ov073_02120ed0(void);
void func_ov073_0212122c(void);
void func_ov073_0212128c(void);
void func_ov073_02121378(void);
void func_ov073_02121388(void);
void func_ov073_02121538(void);
void func_ov073_0212202c(void);
void func_ov073_02122034(void);
void func_ov073_021220c0(void);
void func_ov073_021221e0(void);
void func_ov073_02122200(void);
void func_ov073_021222c8(void);
void func_ov073_021222ec(void);
void func_ov073_0212239c(void);
/* the forty mounted source records (build/port/host-src/ov073_syms.c) */
extern unsigned char data_ov073_02122f40[], data_ov073_02122f48[], data_ov073_02122f50[];
extern unsigned char data_ov073_02122f58[], data_ov073_02122f60[], data_ov073_02122f68[];
extern unsigned char data_ov073_02122f70[], data_ov073_02122f78[], data_ov073_02122f80[];
extern unsigned char data_ov073_02122f88[], data_ov073_02122f90[], data_ov073_02122f98[];
extern unsigned char data_ov073_02122fa0[], data_ov073_02122fa8[], data_ov073_02122fb0[];
extern unsigned char data_ov073_02122fb8[], data_ov073_02122fc0[], data_ov073_02122fc8[];
extern unsigned char data_ov073_02122fd0[], data_ov073_02122fd8[], data_ov073_02122fe0[];
extern unsigned char data_ov073_02122fe8[], data_ov073_02122ff0[], data_ov073_02122ff8[];
extern unsigned char data_ov073_02123000[], data_ov073_02123008[], data_ov073_02123010[];
extern unsigned char data_ov073_02123018[], data_ov073_02123020[], data_ov073_02123028[];
extern unsigned char data_ov073_02123030[], data_ov073_02123038[], data_ov073_0212310c[];
extern unsigned char data_ov073_02123114[], data_ov073_0212311c[], data_ov073_02123124[];
extern unsigned char data_ov073_0212312c[], data_ov073_02123134[], data_ov073_0212313c[];
extern unsigned char data_ov073_02123144[];
}

static void ov73_seat(unsigned char *rec, unsigned rom, void *host,
                      const char *what)
{
    unsigned *w = (unsigned *)rec;
    if (w[0] != rom) {
        std::fprintf(stderr,
                     "ov073 state seat: %s holds 0x%08x, ROM says 0x%08x\n",
                     what, w[0], rom);
        std::abort();
    }
    if (w[1] != 0) {
        std::fprintf(stderr,
                     "ov073 state seat: %s adj word is 0x%08x, not 0\n",
                     what, w[1]);
        std::abort();
    }
    w[0] = (unsigned)(size_t)host;
}

#define OV73_SEAT(sym, rom, host) \
    ov73_seat(sym, (unsigned)(rom), (void *)&host, #sym " -> " #host)

/* VERIFY THEN REWRITE, and only once: a second pass would find its own host
   addresses where the ROM words should be and abort the binary. Guarded here
   as well as at the bring-up, because all four registry rows call the
   bring-up and three of them share one fill. */
extern "C" void port_ov073_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    /* ChiefChilly: 32 records -> the sixteen tables at 0x02123320.. */
    OV73_SEAT(data_ov073_02122f40, 0x02120ed0, func_ov073_02120ed0);
    OV73_SEAT(data_ov073_02122f48, 0x02120e60, func_ov073_02120e60);
    OV73_SEAT(data_ov073_02122f50, 0x0212000c, func_ov073_0212000c);
    OV73_SEAT(data_ov073_02122f58, 0x0211fc78, func_ov073_0211fc78);
    OV73_SEAT(data_ov073_02122f60, 0x02120098, func_ov073_02120098);
    OV73_SEAT(data_ov073_02122f68, 0x0211fbf4, func_ov073_0211fbf4);
    OV73_SEAT(data_ov073_02122f70, 0x0212005c, func_ov073_0212005c);
    OV73_SEAT(data_ov073_02122f78, 0x02121378, func_ov073_02121378);
    OV73_SEAT(data_ov073_02122f80, 0x0212128c, func_ov073_0212128c);
    OV73_SEAT(data_ov073_02122f88, 0x021200e0, func_ov073_021200e0);
    OV73_SEAT(data_ov073_02122f90, 0x02120910, func_ov073_02120910);
    OV73_SEAT(data_ov073_02122f98, 0x02120ad8, func_ov073_02120ad8);
    OV73_SEAT(data_ov073_02122fa0, 0x02120b78, func_ov073_02120b78);
    OV73_SEAT(data_ov073_02122fa8, 0x02120d80, func_ov073_02120d80);
    OV73_SEAT(data_ov073_02122fb0, 0x02120dec, func_ov073_02120dec);
    OV73_SEAT(data_ov073_02122fb8, 0x02120610, func_ov073_02120610);
    OV73_SEAT(data_ov073_02122fc0, 0x0211fa74, func_ov073_0211fa74);
    OV73_SEAT(data_ov073_02122fc8, 0x02120390, func_ov073_02120390);
    OV73_SEAT(data_ov073_02122fd0, 0x021205f0, func_ov073_021205f0);
    OV73_SEAT(data_ov073_02122fd8, 0x0211fbec, func_ov073_0211fbec);
    OV73_SEAT(data_ov073_02122fe0, 0x02120844, func_ov073_02120844);
    OV73_SEAT(data_ov073_02122fe8, 0x021203ac, func_ov073_021203ac);
    OV73_SEAT(data_ov073_02122ff0, 0x02121538, func_ov073_02121538);
    OV73_SEAT(data_ov073_02122ff8, 0x0211fc70, func_ov073_0211fc70);
    OV73_SEAT(data_ov073_02123000, 0x02121388, func_ov073_02121388);
    OV73_SEAT(data_ov073_02123008, 0x021208e4, func_ov073_021208e4);
    OV73_SEAT(data_ov073_02123010, 0x0212081c, func_ov073_0212081c);
    OV73_SEAT(data_ov073_02123018, 0x0212122c, func_ov073_0212122c);
    OV73_SEAT(data_ov073_02123020, 0x0211fe84, func_ov073_0211fe84);
    OV73_SEAT(data_ov073_02123028, 0x02120c7c, func_ov073_02120c7c);
    OV73_SEAT(data_ov073_02123030, 0x02120c08, func_ov073_02120c08);
    OV73_SEAT(data_ov073_02123038, 0x0211fe8c, func_ov073_0211fe8c);
    /* CccArena: 8 records -> the four tables at 0x021234a0.. */
    OV73_SEAT(data_ov073_0212310c, 0x02122200, func_ov073_02122200);
    OV73_SEAT(data_ov073_02123114, 0x021221e0, func_ov073_021221e0);
    OV73_SEAT(data_ov073_0212311c, 0x021222c8, func_ov073_021222c8);
    OV73_SEAT(data_ov073_02123124, 0x021220c0, func_ov073_021220c0);
    OV73_SEAT(data_ov073_0212312c, 0x021222ec, func_ov073_021222ec);
    OV73_SEAT(data_ov073_02123134, 0x0212239c, func_ov073_0212239c);
    OV73_SEAT(data_ov073_0212313c, 0x0212202c, func_ov073_0212202c);
    OV73_SEAT(data_ov073_02123144, 0x02122034, func_ov073_02122034);
}
