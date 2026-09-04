/* HOST COPY of src/func_ov002_020d869c.cpp -- the VS player-on-player hit
 * resolver, with the Luigi Infection team branch spliced in.
 *
 * WHY A HOST COPY.  func_ov002_020d869c is where a punch / slide-kick / ground-
 * pound between two players resolves: it finds the attacker (r6) the victim (c)
 * was struck by, confirms it is a Player, and applies the ROM's knockback and
 * star-drop. Luigi Infection needs exactly two changes at exactly this point --
 * a Luigi attacker TAGS a survivor instead of knocking it, and a Luigi victim
 * is IMMUNE -- and src/ is byte-frozen, so the change cannot live there. This
 * copy is linked in place of the excluded src TU (commented out in
 * slice_gate10.txt), the same mechanism the minimap and the dispatch remodels
 * already use.
 *
 * BYTE-FAITHFUL FOR EVERY NON-LUIGI CASE, BY CONSTRUCTION.  The body below is a
 * VERBATIM transcription of src/func_ov002_020d869c.cpp -- every declaration,
 * every branch, every constant, in the ROM's own order. The ONLY addition is
 * one guard block, and it is reached only when SM64DS_VS_LUIGI_INFECTION is
 * armed:
 *   - mode OFF (the default, and every battery / capture / normal VS match):
 *     port_luigi_enabled() is a cached 0, the block is skipped, and the
 *     remaining code is the src body unchanged. This is the regression net --
 *     with the mode off this function behaves identically to the frozen src,
 *     which the full battery (run with the mode off) exercises.
 *   - mode ON, neither player infected (survivor vs survivor): the block falls
 *     through and the SAME src body runs, so survivor-vs-survivor knockback and
 *     star-drop are the ROM's, unchanged.
 *   - mode ON, a Luigi is involved: the team rule applies (immunity / tag) and
 *     the src body is not reached.
 * So the critical property -- survivor-vs-survivor combat is exactly ROM-normal
 * -- holds whether the mode is off (skipped) or on (fallen through), and only a
 * hit that actually involves a tagger diverges.
 *
 * c IS THE VICTIM, r6 IS THE ATTACKER.  c carries the id of the actor that hit
 * it at c+0x2f8; r6 = FindWithID(that) is the striker, and c+0x2f4 holds the
 * move flags. So "victim is Luigi" is c, "attacker is Luigi" is r6.
 */
#include "types.h"

struct PVec { s32 x, y, z; ~PVec() {} };

extern "C" {
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern int func_ov002_020d85fc(char *self, char *arg, int flags);
extern int func_ov002_020d8360(void *self, void *other, u32 flags);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(char *self, void *clsn, char *player);
extern int _ZN6Player7IsInAirEv(char *p);
extern int func_ov002_020d9298(char *c);
extern void func_ov002_020db8bc(u8 *p, u8 val);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const Vector3 *pos);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, const Vector3 *pos);

/* Luigi Infection, defined in hal/luigi_infection.cpp. All three are a cached
   compare when the mode is off. */
extern int  port_luigi_enabled(void);
extern int  port_luigi_is_infected_player(const void *p);
extern void port_luigi_tag_player(const void *victim);
}

extern "C" int func_ov002_020d869c(char *c)
{
    char *r6;
    u32 flags;
    u32 f2;
    int t;
    PVec p;
    s32 x;
    s32 y;
    s32 z;

    if (*(s32 *)(c + 0xd0) != 0 || *(u8 *)(c + 0x713) == 0)
        return 0;
    if (*(u32 *)(c + 0x2f8) == 0)
        return 0;
    r6 = _ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x2f8));
    if (r6 == 0 || r6 == c)
        return 0;
    t = (int)(*(u16 *)(r6 + 0xc) == 0xbf);
    if (t == 0)
        return 0;

    /* ---- LUIGI INFECTION team branch (inert unless the mode is armed) -------
     * c and r6 are both confirmed Players here. This is the one point where the
     * attacker and the victim are both known, before any knockback or star-drop
     * has been applied, so it is where the team rule belongs. */
    if (port_luigi_enabled()) {
        if (port_luigi_is_infected_player(c))
            /* IMMUNITY: a Luigi victim is never knocked, star-dropped or tagged.
               This same test makes a Luigi-on-Luigi hit a no-op (both on the
               team), so friendly fire between taggers does nothing. */
            return 0;
        if (port_luigi_is_infected_player(r6)) {
            /* TAG: a survivor struck by a Luigi converts and joins the team; the
               tag REPLACES the star-drop, so the ROM resolution below is not
               run. Return 1 -- a hit landed -- exactly as the flags path does. */
            port_luigi_tag_player(c);
            return 1;
        }
        /* neither infected: survivor vs survivor -> fall through to the exact
           ROM resolution below, unchanged. */
    }

    flags = *(u32 *)(c + 0x2f4) & 0x27fe0;
    if (flags != 0) {
        if (func_ov002_020d85fc(c, r6, flags) == 1)
            return 0;
        f2 = flags & ~0x1000u;
        if (f2 != 0)
            func_ov002_020d8360(c, r6, f2);
    } else {
        if (*(u8 *)(c + 0x709) != 0)
            return 0;
        if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, c + 0x2d4, r6) != 0) {
            if (_ZN6Player7IsInAirEv(r6) == 0)
                return 0;
            if (func_ov002_020d9298(r6) != 0) {
                func_ov002_020db8bc((u8 *)c, 0);
                x = *(s32 *)(c + 0x5c);
                y = *(s32 *)(c + 0x60) + 0x82000;
                z = *(s32 *)(c + 0x64);
                p.x = x;
                p.y = y;
                p.z = z;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, p.x, p.y, p.z);
                _ZN5Sound9PlayBank0EjRK7Vector3(0x12, (const Vector3 *)(c + 0x74));
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x19, (const Vector3 *)(c + 0x74));
            }
        }
    }
    return 1;
}
