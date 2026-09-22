/* HOST COPIES of src/_ZN13PrincessPeach12UpdateLookAtEv.cpp and src/_ZN13PrincessPeach21UpdateGroundCollisionEP10dBgCh_Actr.cpp
 * -- two of PRINCESS_PEACH's (actor 186, ov085) per-frame helpers. Neither is
 * a behaviour change and neither is an ABI question: each is a MEASURED MSVC
 * FRONT-END REFUSAL that the matched source cannot be edited out of, because
 * src/ is byte-verified and this port never edits it.
 *
 * ---- _ZN13PrincessPeach12UpdateLookAtEv: C2733, the declaration shape ---------------------
 *
 *     src\_ZN13PrincessPeach12UpdateLookAtEv.cpp(16): error C2733:
 *       '_ZN13PrincessPeach12UpdateLookAtEv': you cannot overload a function with
 *       'extern "C"' linkage
 *
 * include/decl_common.h:1622 declares it `extern int _ZN13PrincessPeach12UpdateLookAtEv(void*)`
 * inside its extern "C" block; the TU includes that header and then defines the
 * body `extern "C" void _ZN13PrincessPeach12UpdateLookAtEv(Actor *self)`. One register and one
 * ignored r0 on the ROM, two C-linkage overloads to MSVC. This is the
 * func_ov102_0214b248 / _ZN8daEyBm_c10HurtPlayerEv case exactly, and the tree's own
 * remedy for it is hostgen.py's HEADER_SHADOW table -- a one-line entry plus a
 * generation block, which hides the header's declaration of just that name so
 * neither src/ nor include/ has to move. THAT IS THE RECLAIM ROUTE FOR THIS
 * BODY and it is cheaper than this file; it was not taken here because
 * HEADER_SHADOW is a shared table and this wave has several lanes in flight.
 * A later lane should take it and delete this half.
 *
 * The body below is the matched TU line for line, with the header include
 * dropped and the two names it wanted from it declared locally.
 *
 * THE RETURN IS VOID BECAUSE THE MATCHED TU'S IS, and the header is the thing
 * that is wrong: config/arm9/overlays/ov085/relocs.txt records exactly ONE
 * caller of this address in the whole ROM -- `from:0x0212a53c kind:arm_call
 * to:0x02129dbc` -- which is inside PrincessPeach::Behavior, and that call
 * discards r0. Nothing reads a return that decl_common.h invented.
 *
 * ---- _ZN13PrincessPeach21UpdateGroundCollisionEP10dBgCh_Actr: C4716, an int body that falls off the end --------
 *
 *     src\_ZN13PrincessPeach21UpdateGroundCollisionEP10dBgCh_Actr.cpp(30) : error C4716:
 *       '_ZN13PrincessPeach21UpdateGroundCollisionEP10dBgCh_Actr': must return a value
 *
 * mwccarm lets a non-void function end without a return -- the value is
 * whatever r0 held -- and MSVC makes that a hard error. The ROM agrees that
 * nothing meant anything by it: at 0x02129f88 the last thing in r0 is the
 * return of SurfaceInfo::CopyNormalTo, and on the early-out path at 0x02129f60
 * it is the zero dBgCh_Actr::IsOnWall just returned.
 *
 * THE VALUE IS DEAD AT EVERY READER, measured rather than assumed. relocs.txt
 * records exactly TWO callers -- `from:0x0212a0d0` (_ZN13PrincessPeach10InitState0Ev, state
 * 4's Main) and `from:0x0212a1bc` (_ZN13PrincessPeach10InitState4Ev, state 2's Main) -- and
 * both spell it as a statement and drop r0. So `return 0` here is a value no
 * caller can distinguish from the ROM's, which is a narrower claim than the
 * ov070 FALLS_OFF_RETURN entry had to make.
 *
 * hostgen.py's FALLS_OFF_RETURN table is the reclaim route for this half, for
 * the reason the paragraph above gives for the other.
 */

// PORT_HOST_ABI: MSVC front-end refusals in the matched TUs (C2733 declaration
// overload, C4716 int body falling off the end). Behaviour unchanged.

struct Vector3 { int x, y, z; };

extern "C" {

int Vec3_HorzDist(const void *a, const void *b);
short Vec3_HorzAngle(const void *a, const void *b);
short Vec3_VertAngle(const void *a, const void *b);
void _Z14ApproachLinearRsss(short *dst, short target, short rate);
int AngleDiff(int a, int b);

}  /* extern "C" */

/* The matched TU's own local shadow, kept verbatim: Actor::ClosestPlayer is a
   thiscall reached THROUGH A RECEIVER here, which is why this body is not a
   closestplayer_guard row. hal/reverse_bridges.cpp defines the MSVC name that
   shape mangles to (?ClosestPlayer@dActor_c@@QAEPAU1@XZ). */
struct Actor { Actor *ClosestPlayer(); };

/* PORT_HOST_ABI: MSVC front-end refusal C2733 (declaration overload) in the
   matched TU. Behaviour unchanged. */
extern "C" void _ZN13PrincessPeach12UpdateLookAtEv(void *selfv)
{
    Actor *self = (Actor *)selfv;
    char *s = (char *)self;
    Actor *p = self->ClosestPlayer();
    if (p == 0)
        return;
    Vector3 v;
    Vector3 *psrc = (Vector3 *)(((long long)(int)((char *)p + 0x5c)));
    v = *psrc;
    int hd = Vec3_HorzDist(s + 0x5c, &v);
    v.y = v.y - 0x1e000;
    short ha = Vec3_HorzAngle(s + 0x5c, &v);
    short va = Vec3_VertAngle(s + 0x5c, &v);
    if (hd < 0x15e000 && AngleDiff(ha, *(short *)(s + 0x8e)) < 0x3000) {
        *(short *)(s + 0x364) = va;
        *(short *)(s + 0x366) = *(short *)(s + 0x8e) - ha;
    } else {
        *(short *)(s + 0x364) = 0;
        *(short *)(s + 0x366) = 0;
    }
    _Z14ApproachLinearRsss((short *)(s + 0x362), *(short *)(s + 0x366), 0x250);
    _Z14ApproachLinearRsss((short *)(s + 0x360), *(short *)(s + 0x364), 0x100);
}

extern "C" {

int dBgCh_Actr_UpdateContinuous_Veneer(void* c);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* c);
void* _ZNK10dBgCh_Actr14GetFloorResultEv(void* c);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, int* out);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK10dBgCh_Actr8IsOnWallEv(void* c);
void* _ZNK10dBgCh_Actr13GetWallResultEv(void* c);

/* PORT_HOST_ABI: MSVC front-end refusal C4716 (int body falling off the end)
   in the matched TU. The dropped return value is dead at both callers. */
int _ZN13PrincessPeach21UpdateGroundCollisionEP10dBgCh_Actr(int* self, void* clsn)
{
    int n0[3];
    int n1[3];
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn)+4, n0);
        if (n0[1] != 0) {
            long long a = (long long)n0[0] * (long long)self[0xa4/4];
            long long b = (long long)n0[2] * (long long)self[0xac/4];
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            self[0xa8/4] = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
        }
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4, n1);
    }
    return 0;   /* the ROM leaves r0 undefined here; dead at both callers */
}

}  /* extern "C" */
