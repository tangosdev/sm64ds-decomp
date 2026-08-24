//cpp
// @symbol _ZN7daTrs_c8BehaviorEv
/* daTrs_c::Behavior (slot 6, fBase_c) -- ov063 0x0211b0a4, 0x7e4 bytes.
 * Recovered from the banked near-miss C draft (nearmiss/db.jsonl, floored at
 * 54 divergences as a free C function) and finished as a real C++ method; the
 * last regallocation walls fell to spelling choices, not new semantics:
 *
 *   - The respawn-position block reads the 0x5xx fields with direct
 *     (c + 0x5d0/0x570/...) spellings so the compiler synthesizes the
 *     r+0x500 base itself (an explicit `base = c + 0x500` local colors the
 *     base wrong), and copies the 0x92 angle triple through
 *     `s16 (*ap)[3] = (s16 (*)[3])(c + 0x92)` -- the array-pointer deref is
 *     the one spelling that makes that base outrank the copy scratch.
 *   - The show-hide block wants the opposite ranking: plain pointer locals
 *     (`r2 = c + 0x92; fp = (u16 *)(c + 0x5d4);`) declared before the
 *     position stores, with the same direct 0x5xx loads.
 *   - `*(u16 *)(c + 0x100) += 1;` -- the compound form colors the
 *     address/value pair the way the ROM has it; the spelled-out
 *     load-add-store form colors it backwards.
 *   - The closest-player Vector3 copy must be memberwise through an s32*
 *     (three load/store pairs interleave); going through locals first merges
 *     the base add into a pre-indexed ldr and drops an instruction.
 *   - In the raycast setup the y+0x32000 sum must be computed into its own
 *     local BETWEEN the z and x loads (creation order drives the r3/ip
 *     coloring of x vs the sum).
 *   - The talk-state counter increments and the flag RMWs go through plain
 *     pointer locals (`bp = (u8 *)(c + 0x5d1);` etc.) -- the RMW through a
 *     pointer variable materializes the address (0x5d1/0x5d4 are not ARM
 *     rotate-immediates, so the compiler spells them via the literal pool,
 *     exactly the ROM's shape). The C draft needed `& 0xFFFF...` laundering
 *     masks for the same effect; as a real method none of them are needed.
 *
 * unk_* raw offsets beyond the typed members follow the same convention as
 * the sibling daTrs_c files (see _ZN7daTrs_c6RenderEv.cpp). */
#include "daTrs_c.h"
#include "dBgCh_Gnd.h"

extern "C" {
void func_0200f760(void *thiz, void *cyl);
void *_ZN8dActor_c10FindWithIDEj(u32 id);
int _ZN11dCapEnemy_c11GetCapStateEv(char *c);
int _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(char *c, void *w, void *m, u32 j);
void _ZN8dActor_c8PoofDustEv(char *c);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(char *c);
void func_0201267c(u32 a, void *b);
void *_ZN11dCapEnemy_c15RespawnIfHasCapEv(char *c);
u8 IsAreaShowing(s8 idx);
int func_ov063_02116190(char *c);
void _ZN11dCapEnemy_c12Unk_02005d94Ev(char *c);
int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(char *c, void *w);
int _ZN11dCapEnemy_c16GetCapEatenOffItERK7Vector3(char *c, const Vector3 *v);
void func_ov063_02119ab0(char *c);
int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *plr, void *base, u32 id, const void *pos, u32 e, u32 f);
int _ZN6Player12GetTalkStateEv(void *plr);
void _ZN6Player9DropActorEv(void *plr);
void func_ov063_021166ac(char *c);
void _ZN5dCc_c5ClearEv(void *cyl);
void *_ZN8dActor_c13ClosestPlayerEv(char *c);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
s32 Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void func_ov063_021192d4(char *c);
void func_ov063_02116bf4(char *c);
void func_ov063_02116a1c(char *c);
void func_ov063_02116d38(char *c);
void func_ov063_0211ab68(char *c);
void func_ov063_021189f4(char *c);
void func_ov063_021172a8(char *c);
void func_ov063_02119274(char *c);
void func_ov063_02116fac(char *c);
u16 DecIfAbove0_Short(void *p);
void _ZN8dActor_c9UpdatePosEP5dCc_c(char *c, void *cyl);
void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd *rc);
void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *rc, const Vector3 *v, void *actor);
int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *rc);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(char *c, void *w, u32 j);
void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *rc);
void _ZN9Animation7AdvanceEv(void *anim);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *cyl, void *v);
void _ZN5dCc_c6UpdateEv(void *cyl);
}

int daTrs_c::Behavior()
{
    char *c = (char *)this;
    Vector3 pv;
    Vector3 v1;
    Vector3 v2;
    Vector3 ve;
    /* Dumb word storage, not typed locals: rc1/rc2 are CONSTRUCTED
       MID-FUNCTION, rc2 only on some paths (notes/ctor-migration.md item 9).
       A typed local of a class with a declared constructor is constructed at
       its declaration -- the ROM does not do that here. Call sites cast the
       array's address, which costs exactly the sp-relative add the old POD
       local spelled. */
    u32 rc1[sizeof(dBgCh_Gnd) / sizeof(u32)];
    u32 rc2[sizeof(dBgCh_Gnd) / sizeof(u32)];
    int t;
    void *p;
    char *q;
    char *r1;
    char *r2;
    s32 *p19c;
    u16 *fp;
    u8 *bp;
    s32 y;
    s32 w;
    s32 z;
    s32 x;
    int d1;
    int v;

    func_0200f760(c, c + 0x184);
    if ((*(u32 *)(c + 0x49c)) != 0) {
        *(void **)(c + 0x48c) = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x49c));
        q = *(char **)(c + 0x48c);
        if (q != 0) {
            *(s32 *)(q + 0x5c) = *(s32 *)(c + 0x5c);
            *(s32 *)(q + 0x60) = *(s32 *)(c + 0x60);
            *(s32 *)(q + 0x64) = *(s32 *)(c + 0x64);
        }
        *(void **)(c + 0x48c) = 0;
    }
    if (_ZN11dCapEnemy_c11GetCapStateEv(c) == 0)
        return 1;

    t = _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(c, c + 0x1c4, c + 0x380, 0);
    if (t != 0) {
        if (t == 2) {
            _ZN8dActor_c8PoofDustEv(c);
            if ((*(s32 *)(c + 0x5a4)) != 0) {
                _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
                func_0201267c(0xd5, c + 0x74);
                if (((*(u8 *)(c + 0x113)) & 0xf) < 6) {
                    *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x51c);
                    *(s32 *)(c + 0x60) = *(s32 *)(c + 0x520);
                    *(s32 *)(c + 0x64) = *(s32 *)(c + 0x524);
                    *(s8 *)(c + 0xcc) = *(s8 *)(c + 0x5d0);
                    *(s16 *)(c + 0x92) = *(s16 *)(c + 0x570);
                    *(s16 *)(c + 0x94) = *(s16 *)(c + 0x572);
                    *(s16 *)(c + 0x96) = *(s16 *)(c + 0x574);
                    {
                        s16 (*ap)[3] = (s16 (*)[3])(c + 0x92);
                        *(s16 *)(c + 0x8c) = (*ap)[0];
                        *(s16 *)(c + 0x8e) = (*ap)[1];
                        *(s16 *)(c + 0x90) = (*ap)[2];
                    }
                    p = _ZN11dCapEnemy_c15RespawnIfHasCapEv(c);
                    if (p != 0) {
                        fp = (u16 *)((char *)p + 0x5d4);
                        *fp = (*fp) & (~2);
                    }
                }
            } else {
                *(u8 *)(c + 0x5cc) = 5;
                *(u8 *)(c + 0x5ce) = 0;
                p19c = (s32 *)(c + 0x19c);
                *p19c = (*p19c) | 1;
                fp = (u16 *)(c + 0x5d4);
                *fp = (*fp) & (~8);
            }
        }
        return 1;
    }

    *(s8 *)(c + 0xcc) = -1;
    if ((*(u16 *)(c + 0x5c6)) == 0) {
        if ((IsAreaShowing(*(s8 *)(c + 0x5d0)) == 0) || (func_ov063_02116190(c) != 0)) {
            if ((((u32)((*(u16 *)(c + 0x5d4)) << 0x18)) >> 0x1f) == 0) {
                r2 = c + 0x92;
                *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x51c);
                fp = (u16 *)(c + 0x5d4);
                *(s32 *)(c + 0x60) = *(s32 *)(c + 0x520);
                *(s32 *)(c + 0x64) = *(s32 *)(c + 0x524);
                *(s16 *)(c + 0x92) = *(s16 *)(c + 0x570);
                *(s16 *)(c + 0x94) = *(s16 *)(c + 0x572);
                *(s16 *)(c + 0x96) = *(s16 *)(c + 0x574);
                *(s16 *)(c + 0x8c) = *(s16 *)r2;
                *(s16 *)(c + 0x8e) = *(s16 *)(r2 + 2);
                *(s16 *)(c + 0x90) = *(s16 *)(r2 + 4);
                *fp = (*fp) | 0x10;
                *(u8 *)(c + 0x5cc) = 0;
                *(s32 *)(c + 0x98) = 0;
                *(s32 *)(c + 0x9c) = 0;
                *(s32 *)(c + 0xa8) = 0;
                *(u8 *)(c + 0x5ce) = 0;
                *(s8 *)(c + 0xcc) = *(s8 *)(c + 0x5d0);
            }
            return 1;
        }
    }
    _ZN11dCapEnemy_c12Unk_02005d94Ev(c);
    t = _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(c, c + 0x1c4);
    if (t != 0) {
        if (t == 1) {
            ve.x = *(s32 *)(c + 0x564);
            ve.y = *(s32 *)(c + 0x568);
            ve.z = *(s32 *)(c + 0x56c);
            if (_ZN11dCapEnemy_c16GetCapEatenOffItERK7Vector3(c, &ve) != 0)
                return 1;
        }
        *(s32 *)(c + 0x9c) = -0x2000;
        func_ov063_02119ab0(c);
        if ((*(u8 *)(c + 0x107)) != 0) {
            u16 *p100 = (u16 *)(c + 0x100);
            if (p100[2] == 0) {
                *(u8 *)(c + 0x107) = 0;
                ((u16 *)(c + 0x100))[2] = 0;
                *(s32 *)(c + 0x9c) = 0;
                *(s32 *)(c + 0xa8) = 0;
                goto block_39;
            }
        }
        v = ((*(s32 *)(c + 0xb0)) & 0x40000) ? 1 : 0;
        if (v != 0) {
            u8 st = *(u8 *)(c + 0x5d1);
            char *pl = *(char **)(c + 0xd0);
            switch (st) {
            case 0:
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(pl, c, 0x15a, 0, 0, 2) != 0) {
                    bp = (u8 *)(c + 0x5d1);
                    *bp = (*bp) + 1;
                    func_0201267c(0xf8, c + 0x74);
                }
                break;
            case 1:
                if (_ZN6Player12GetTalkStateEv(pl) == -1) {
                    _ZN6Player9DropActorEv(pl);
                    bp = (u8 *)(c + 0x5d1);
                    *bp = (*bp) + 1;
                }
                break;
            }
        }
        func_ov063_021166ac(c);
        _ZN5dCc_c5ClearEv(c + 0x184);
        return 1;
    }
block_39:
    *(u8 *)(c + 0x5d1) = 0;

    {
        void *plr = _ZN8dActor_c13ClosestPlayerEv(c);
        *(void **)(c + 0x484) = plr;
        plr = *(void **)(c + 0x484);
        if (plr != 0) {
            s32 *pp = (s32 *)(((char *)plr) + 0x5c);
            pv.x = pp[0];
            pv.y = pp[1];
            pv.z = pp[2];
            *(s16 *)(c + 0x5b0) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &pv);
            *(s32 *)(c + 0x580) = Vec3_HorzDist((Vector3 *)(c + 0x5c), &pv);
        } else {
            r1 = c + 0x500;
            *(s16 *)(r1 + 0xb0) = *(s16 *)(c + 0x8e);
            *(s32 *)(c + 0x580) = 0x2710000;
        }
    }
    *(u8 *)(c + 0x5cd) = *(u8 *)(c + 0x5cc);
    switch (*(u8 *)(c + 0x5cf)) {
    case 0:
    case 1:
    case 2:
        func_ov063_021192d4(c);
        break;
    case 3:
        func_ov063_02116bf4(c);
        break;
    case 4:
        func_ov063_02116a1c(c);
        break;
    case 5:
        func_ov063_02116d38(c);
        break;
    case 6:
    case 10:
        func_ov063_021192d4(c);
        break;
    case 7:
        func_ov063_0211ab68(c);
        break;
    case 12:
    case 13:
    case 14:
        func_ov063_021189f4(c);
        break;
    case 15:
        func_ov063_021172a8(c);
        break;
    case 8:
        func_ov063_02119274(c);
        break;
    case 9:
        func_ov063_021192d4(c);
        break;
    case 11:
        func_ov063_02116fac(c);
    }

    if ((((u32)((*(u16 *)(c + 0x5d4)) << 0x1d)) >> 0x1f) != 0)
        *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
    *(u16 *)(c + 0x100) += 1;
    DecIfAbove0_Short(c + 0x5c0);
    if ((*(u8 *)(c + 0x5cd)) != (*(u8 *)(c + 0x5cc)))
        *(u16 *)(c + 0x100) = 0;
    if ((*(u8 *)(c + 0x5cf)) != 3) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x184);
        func_ov063_02119ab0(c);
        if (((((u32)((*(u16 *)(c + 0x5d4)) << 0x1f)) >> 0x1f) != 0) && ((*(s32 *)(c + 0x64)) < -0x12c000))
            *(s32 *)(c + 0x64) = -0x12c000;
        _ZN9dBgCh_GndC1Ev((dBgCh_Gnd *)rc1);
        y = *(s32 *)(c + 0x60);
        z = *(s32 *)(c + 0x64);
        w = y + 0x32000;
        x = *(s32 *)(c + 0x5c);
        v1.x = x;
        v1.y = w;
        v1.z = z;
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c((dBgCh_Gnd *)rc1, &v1, c);
        if (_ZN9dBgCh_Gnd10DetectClsnEv((dBgCh_Gnd *)rc1) != 0) {
            s32 ground = (*(s32 *)((char *)rc1 + 0x44)) + 0x2000;
            if ((*(s32 *)(c + 0x60)) < ground)
                *(s32 *)(c + 0x60) = ground;
        }
        if (((*(u8 *)(c + 0x5cf)) != 4) && ((*(u8 *)(c + 0x5cf)) != 0xb)) {
            _ZN9dBgCh_GndC1Ev((dBgCh_Gnd *)rc2);
            y = *(s32 *)(c + 0x60);
            z = *(s32 *)(c + 0x64);
            w = y + 0x32000;
            x = *(s32 *)(c + 0x5c);
            v2.x = x;
            v2.y = w;
            v2.z = z;
            _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c((dBgCh_Gnd *)rc2, &v2, c);
            d1 = (int)((*(u16 *)(c + 0xc)) == 0xd1);
            if (d1 != 0) {
                if ((*(u8 *)(c + 0x5cf)) < 8) {
                    if ((((u32)((*(u16 *)(c + 0x5d4))) << 0x1a) >> 0x1f) != 0) {
                        if ((_ZN9dBgCh_Gnd10DetectClsnEv((dBgCh_Gnd *)rc2) == 0) ||
                            (((*(s32 *)(c + 0x60)) - (*(s32 *)((char *)rc2 + 0x44))) > 0x12c000)) {
                            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x528);
                            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x52c);
                            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x530);
                        } else {
                            *(s32 *)(c + 0x528) = *(s32 *)(c + 0x5c);
                            *(s32 *)(c + 0x52c) = *(s32 *)(c + 0x60);
                            *(s32 *)(c + 0x530) = *(s32 *)(c + 0x64);
                        }
                    } else {
                        goto ray_e;
                    }
                } else {
                    goto ray_e;
                }
            } else {
            ray_e:
                if ((_ZN9dBgCh_Gnd10DetectClsnEv((dBgCh_Gnd *)rc2) != 0) &&
                    (((*(s32 *)(c + 0x60)) - (*(s32 *)((char *)rc2 + 0x44))) < 0x12c000)) {
                    fp = (u16 *)(c + 0x5d4);
                    *fp = (*fp) | 0x20;
                    *(s32 *)(c + 0x528) = *(s32 *)(c + 0x5c);
                    *(s32 *)(c + 0x52c) = *(s32 *)(c + 0x60);
                    *(s32 *)(c + 0x530) = *(s32 *)(c + 0x64);
                }
            }
            _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, c + 0x1c4, 0);
            _ZN9dBgCh_GndD1Ev((dBgCh_Gnd *)rc2);
        }
        _ZN9dBgCh_GndD1Ev((dBgCh_Gnd *)rc1);
    }
    if (((((*(u8 *)(c + 0x5cc)) != 3) && ((*(u8 *)(c + 0x5cc)) != 3)) && ((*(u8 *)(c + 0x5cc)) != 3)) && ((*(u8 *)(c + 0x5cc)) != 3))
        _ZN9Animation7AdvanceEv(c + 0x3d0);
    func_ov063_021166ac(c);
    _ZN5dCc_c5ClearEv(c + 0x184);
    if ((*(u8 *)(c + 0x5c8)) == 0xff) {
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x184, c + 0x534);
        _ZN5dCc_c6UpdateEv(c + 0x184);
    }
    return 1;
}
