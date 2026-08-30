/* PORT_HOST_ABI. SKEETER (230, 9daMenbo_c), ov090. Run rel0215 wave 2, lane
 * cast-ov090.
 *
 * Three sites MSVC cannot deliver from src/, each refused for a named reason.
 * The byte-matched originals stay in src/ and are dropped from the CMake
 * source list by port/slice_ov090.txt; nothing here edits src/.
 *
 * (1) src/_ZN7Skeeter6RenderEv.cpp -- THE ModelAnim SLOT-5 COLLISION, the
 *     Whomp/Fish/Spiny case (T1). The source draws through a six-virtual local
 *     shadow over the embedded ModelAnim at +0x30c, so it reaches index 5 in
 *     MSVC's own numbering; but hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in
 *     MSVC numbering, where slot 5 is Virtual18 and Render has moved. The host
 *     copy names ModelAnim::Render outright. Offsets are Skeeter.h's own:
 *     unk flags 0xb0, mScaleX 0x80, mModelAnim 0x30c. The scale argument is
 *     &mScaleX, not 0 -- Skeeter is the only one of the four water classes
 *     that passes one, and it is read off the source, not assumed.
 *
 * (2) src/func_ov090_02131e00.cpp -- THE PMF DISEASE, the SoundObject / Cap /
 *     MrBlizzard / BabyPenguin / Unagi / HootTheOwl / Lakitu shape. The source
 *     forms
 *         struct C; typedef int (C::*PMF)();
 *         struct C { char pad[0x370]; PMF *pp; };
 *         c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)();
 *     -- a real C++ pointer-to-member on a DELIBERATELY INCOMPLETE class.
 *     MSVC's representation for a pointer-to-member of an incomplete class is
 *     the "unknown inheritance" general form, four words, not the ROM's own
 *     8-byte {function, delta} record. This is the STATE SETTER: it stores the
 *     cell pointer at self+0x370 and then dispatches record 0, the ENTER half.
 *     Every delta word in ov090's sixteen PMF source records is 0 in the ROM
 *     (verified against overlay_0090.bin, see port/ov090_syms.txt), so a plain
 *     call through the fn word is the same call.
 *
 * (3) src/_ZN7Skeeter8BehaviorEv.cpp -- the same disease at the TICK half.
 *     Its `PmfNode { char pad[8]; ActorFn fn; }` reaches record 1 of whichever
 *     cell self+0x370 points at. Transcribed body-for-body from the matched
 *     source with ONLY that call replaced; every other statement, offset and
 *     constant is the source's.
 *
 * THE SEAT that makes (2) and (3) legal is port_ov090_states_seat() in
 * hal/actor_classes_ov090.cpp: the mounted PMF SOURCE records hold DS CODE
 * addresses (ovdata.py's pointer pass only rebases pointers into other mounted
 * DATA), so each record's fn word is rewritten with its HOST body's address --
 * validated against the ROM's own address first -- BEFORE
 * __sinit_ov090_02133ce8 copies Skeeter's eight records into the four bss
 * cells data_ov090_021344e4 / _021344f4 / _02134504 / _02134514.
 */
#include "ModelAnim.h"

extern "C" {

/* ---- (1) Render -------------------------------------------------------- */
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch. */
int _ZN7Skeeter6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if ((*(unsigned int *)(c + 0xb0) & 0x40000) != 0)
        return 1;
    ((ModelAnim *)(c + 0x30c))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    return 1;
}

/* ---- (2) and (3) the two state dispatch sites -------------------------- */
struct PortOv090Pmf { unsigned int fn; int delta; };
typedef int (*PortOv090StateFn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
int func_ov090_02131e00(void *cv, void *pv)   /* record 0, the ENTER half */
{
    *(void **)((char *)cv + 0x370) = pv;
    PortOv090Pmf *q = (PortOv090Pmf *)pv;
    if (q[0].fn == 0)
        return 1;
    return ((PortOv090StateFn)(size_t)q[0].fn)(cv);
}

/* ---- (3) Behavior, transcribed with the record-1 dispatch hosted -------- */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
struct SkVec3 { s32 x, y, z; };

int  _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *self, void *wm);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
void func_ov090_02131e50(void *c);
int  _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void *self, void *wm, void *anim, u32 n);
void func_02035684(int *p, int v);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm, u32 n);
int  _ZN5Enemy11UpdateDeathER12WithMeshClsn(void *self, void *wm);
void func_ov090_02131378(void *c);
void _ZN5Actor8PoofDustEv(void *self);
void _Z14ApproachLinearRsss(s16 *p, s16 a, s16 b);
int  AngleDiff(int a, int b);
void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *v, u32 n, s32 fix, s16 s);
void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cyl);
int  func_02035638(u8 *p);
void *_ZN5Actor13ClosestPlayerEv(void *self);
void func_020aea30(void *self, void *p, int n);
u16  DecIfAbove0_Short(u16 *p);
void _ZN9Animation7AdvanceEv(void *self);
void func_ov090_021310b4(void *c);
extern signed char data_0209f2f8;

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class (the
 * record-1 tick dispatch); everything else is the matched source verbatim. */
int _ZN7Skeeter8BehaviorEv(void *self)
{
    char *c = (char *)self;

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x150)) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(u8 *)(c + 0x107) != 0 && *(u16 *)(c + 0x104) == 0)
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
        func_ov090_02131e50(c);
        return 1;
    }

    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, c + 0x150, c + 0x30c, 3))
        return 1;

    if (*(s32 *)(c + 0x10c) != 0) {
        func_02035684((int *)(c + 0x150), 0xd2000);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x150, 0);
        if (_ZN5Enemy11UpdateDeathER12WithMeshClsn(c, c + 0x150))
            return 1;
        func_ov090_02131378(c);
        func_ov090_02131e50(c);
        if (*(s32 *)(c + 0x10c) == 0)
            _ZN5Actor8PoofDustEv(c);
        if (*(u8 *)(c + 0x3a1) == 3) {
            _Z14ApproachLinearRsss((s16 *)(c + 0x8c), -32767, 0x500);
            if (AngleDiff(*(s16 *)(c + 0x8c), -32767) < 0x1000) {
                s16 *p8e = (s16 *)(((int)c + 0x8e));
                *p8e += 0x1000;
            }
        }
        if (*(s32 *)(c + 0x10c) != 1 && *(s32 *)(c + 0x60) <= *(s32 *)(c + 0x3ac)) {
            SkVec3 v;
            v.x = *(s32 *)(c + 0x5c);
            v.y = *(s32 *)(c + 0x60);
            v.z = *(s32 *)(c + 0x64);
            _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, *(u8 *)(c + 0x10a) + 1, 0xa000, 0);
            _ZN5Actor8PoofDustEv(c);
            _ZN5Actor24KillAndTrackInDeathTableEv(c);
        }
        return 1;
    }

    {
    int flag = (*(s32 *)(c + 0xb0) & 8) != 0;
    if (flag) {
        *(s32 *)(c + 0x98) = 0;
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
        func_ov090_02131378(c);
        if (data_0209f2f8 == 0x15 && *(signed char *)(c + 0xcc) == 1) {
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x150, 2);
            if (func_02035638((u8 *)(c + 0x150))) {
                *(s32 *)(c + 0x9c) = 0;
                *(s32 *)(c + 0xa4) = 0;
                *(s32 *)(c + 0xa8) = 0;
                *(s32 *)(c + 0xac) = 0;
                *(s32 *)(c + 0x10c) = 1;
                func_020aea30(c, _ZN5Actor13ClosestPlayerEv(c), 0);
                return 1;
            }
        }
        return 1;
    }
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    func_ov090_02131378(c);
    DecIfAbove0_Short((u16 *)(c + 0x100));
    DecIfAbove0_Short((u16 *)(c + 0x394));
    DecIfAbove0_Short((u16 *)(c + 0x396));
    DecIfAbove0_Short((u16 *)(c + 0x398));
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x150, 2);
    if (*(s32 *)(c + 0x60) <= *(s32 *)(c + 0x3ac))
        *(s32 *)(c + 0x60) = *(s32 *)(c + 0x3ac);
    if (data_0209f2f8 == 0x15 && *(signed char *)(c + 0xcc) == 1 && func_02035638((u8 *)(c + 0x150))) {
        *(s32 *)(c + 0x9c) = 0;
        *(s32 *)(c + 0xa4) = 0;
        *(s32 *)(c + 0xa8) = 0;
        *(s32 *)(c + 0xac) = 0;
        *(s32 *)(c + 0x10c) = 1;
        func_020aea30(c, _ZN5Actor13ClosestPlayerEv(c), 0);
        return 1;
    }

    {
        PortOv090Pmf *n = *(PortOv090Pmf **)(c + 0x370);
        if (n[1].fn)
            ((PortOv090StateFn)(size_t)n[1].fn)(c);
    }
    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
    *(s32 *)(c + 0x368) = *(s32 *)(c + 0x3a4);
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02131e50(c);
    func_ov090_021310b4(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        void *p = _ZN5Actor13ClosestPlayerEv(c);
        if (p != 0 && *(u8 *)((char *)p + 0x6fb) == 0)
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
    }
    return 1;
}

}  /* extern "C" */
