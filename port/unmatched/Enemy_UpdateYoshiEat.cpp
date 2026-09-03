/* HOST COPY of Enemy::UpdateYoshiEat (dEnemyBase_c::UpdateYoshiEat),
 * ov002 0x020ade78, 0x3cc bytes -- the eaten actor's own state machine while
 * it is inside Yoshi's tongue/mouth and during the ballistic spit-out.
 *
 * WHY THIS IS A HOST COPY AND NOT MATCHED SRC.
 *
 * The function is UNMATCHED and, per the recovered draft in nearmiss/db.jsonl,
 * NOT MATCHABLE in this lane: 241 of 243 instructions are byte-exact and the
 * residual is exactly the 6o ordering floor ("speculative stall-slot fill") --
 * at +0x174/+0x17c the ROM fills the d0-load delay slot with 'mov r3,#0x800'
 * then 'add r1,r6,#0x5c', and mwcc invariably hoists the add first. ~10
 * spellings plus 7000+ decomp-permuter iterations at score 120 failed, the same
 * as func_ov006_020dac34 in the 6o floor. So it stays out of src/ (src/ must
 * hold only byte-exact bodies) and lives here as a faithful host transcription.
 *
 * The u64-launder casts the draft carried ( ((long long)(int)p) & ~0 ) are pure
 * mwcc scheduling hacks -- semantic no-ops that reduce to the plain pointer --
 * and are dropped here; MSVC compiles the plain form and this copy is
 * behaviour-for-behaviour the draft, which is the ROM up to that one swap.
 *
 * WHAT IT DOES (the bug this fixes -- Yoshi-swallow-key, known since 0.1.11).
 *
 * Every enemy Behavior that Yoshi can eat opens by calling this on itself with
 * its WithMeshClsn subobject (self+0x144). It reads the eat-state bits in
 * self+0xb0 and returns:
 *   0  not being eaten            caller runs its normal AI
 *   1  bit 0x20000                held, early sub-state
 *   2  bit 0x40000                grabbed: track the eater's position (+0x30000
 *                                 up), self+0xd0 is the eater
 *   3  bit 0x80000 / spit active  SWALLOW-COMPLETION: launch the enemy
 *                                 ballistically out of Yoshi (angle table
 *                                 data_02082214, speed from the eater), then
 *                                 fly + ground-bounce until it settles
 * A non-zero return is what makes each caller enter its eat branch instead of
 * its normal AI. For the intro rabbit (Rabbit::Behavior, src/_ZN6Rabbit8Behav-
 * iorEv.c:130) that branch is what writes rabbit+0x45c (the caught-by player),
 * which arms the talk block (line 85) and the key spawn. With the old host stub
 * returning a constant 0 the branch was dead: an eaten enemy never entered the
 * swallow states, never handed over its reward, and the rabbit's key/dialogue
 * never fired. Un-stubbing this restores the whole path.
 *
 * The old constant-0 stub in hal/actor_vtables.cpp is removed; this definition
 * replaces it and rides the LEVELBOOT_HOST_SOURCES slice, next to
 * OneUpMushroom_Behavior.cpp, exactly the targets that carry the enemy Behaviors
 * that call it. The small actor smokes that also link actor_vtables.cpp never
 * reference the symbol (their slices carry no enemy Behavior), so dropping the
 * stub costs them nothing.
 */

typedef unsigned short u16;
typedef short s16;

extern "C" {

int   _ZNK12WithMeshClsn13GetLimMovFlagEv(void *clsn);
void  _ZN12WithMeshClsn13SetLimMovFlagEv(void *clsn);
void  _ZN12WithMeshClsn15ClearLimMovFlagEv(void *clsn);
void  _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cyl);
void  WithMeshClsn_UpdateContinuous_Veneer(void *clsn); /* ov002 called this func_020383fc; the port hosts it under its role name (src/WithMeshClsn_UpdateContinuous_Veneer.c, a tail-jump veneer -- the arg rides the caller's frame into the jump, same as its other callers) */
int   _ZNK12WithMeshClsn10IsOnGroundEv(void *clsn);
char *_ZNK12WithMeshClsn14GetFloorResultEv(void *clsn);
void  _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *surf, void *out);
int   _ZNK12WithMeshClsn13JustHitGroundEv(void *clsn);
int   _ZN4cstd4fdivEii(int a, int b);
int   Vec3_HorzLen(int *v);
extern short data_02082214[];

int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *selfv, void *clsnv)
{
    char *self = (char *)selfv;
    char *clsn = (char *)clsnv;
    int flags = *(int *)(self + 0xb0);

    /* 0x40000: grabbed, ride the eater's mouth */
    if (flags & 0x40000) {
        char *eater = *(char **)(self + 0xd0);
        if (eater == 0) {
            *(int *)(self + 0xb0) &= ~0x40000;
        } else {
            *(int *)(self + 0x5c) = *(int *)(eater + 0x5c);
            *(int *)(self + 0x60) = *(int *)(eater + 0x60);
            *(int *)(self + 0x64) = *(int *)(eater + 0x64);
            *(int *)(self + 0x60) += 0x30000;
        }
        *(unsigned char *)(self + 0x107) = 0;
        return 2;
    }

    /* 0x20000: held, early sub-state */
    if (flags & 0x20000) {
        *(unsigned char *)(self + 0x107) = 0;
        return 1;
    }

    /* 0x80000 or spit-out already active */
    if ((flags & 0x80000) || *(unsigned char *)(self + 0x107) != 0) {
        if (*(u16 *)(self + 0x104) != 0)
            *(u16 *)(self + 0x104) -= 1;

        if (*(int *)(self + 0xb0) & 0x80000) {
            /* swallow-completion: begin the ballistic launch */
            int v;
            char *eater;
            *(unsigned char *)(self + 0x107) = 1;
            *(u16 *)(self + 0x104) = 5;
            *(int *)(self + 0xb0) &= ~0x80000;
            eater = *(char **)(self + 0xd0);
            if (eater == 0) {
                v = 0x14000;
            } else {
                u16 ang;
                int idx;
                s16 *p;
                *(int *)(self + 0x5c) = *(int *)(eater + 0x5c);
                *(int *)(self + 0x60) = *(int *)(eater + 0x60);
                *(int *)(self + 0x64) = *(int *)(eater + 0x64);
                *(int *)(self + 0x60) += 0x32000;

                ang = *(u16 *)(eater + 0x8e);
                idx = (ang >> 4) << 1;
                *(int *)(self + 0x5c) +=
                    (int)(((long long)data_02082214[idx] * 0x32000 + 0x800) >> 12);
                *(int *)(self + 0x64) +=
                    (int)(((long long)data_02082214[idx + 1] * 0x32000 + 0x800) >> 12);

                p = (s16 *)(eater + 0x8c);
                *(s16 *)(self + 0x92) = p[0];
                *(s16 *)(self + 0x94) = p[1];
                *(s16 *)(self + 0x96) = p[2];

                v = *(int *)(eater + 0x98);
            }
            if (v >= 0x14000)
                *(int *)(self + 0x98) = v + 0xa000;
            else
                *(int *)(self + 0x98) =
                    (int)(((long long)v * 0x800 + 0x800) >> 12) + 0x14000;
            *(int *)(self + 0x98) =
                (int)(((long long)*(int *)(self + 0x98) * 0x14cc + 0x800) >> 12);
            *(int *)(self + 0xa8) = 0xc000;
            *(int *)(self + 0xd0) = 0;
        }

        {
            int lim = _ZNK12WithMeshClsn13GetLimMovFlagEv(clsn);
            _ZN12WithMeshClsn13SetLimMovFlagEv(clsn);
            _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
            WithMeshClsn_UpdateContinuous_Veneer(clsn);
            if (_ZNK12WithMeshClsn10IsOnGroundEv(clsn) != 0) {
                char *fr = _ZNK12WithMeshClsn14GetFloorResultEv(clsn);
                _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, self + 0xd4);
                if (_ZNK12WithMeshClsn13JustHitGroundEv(clsn) == 0) {
                    *(int *)(self + 0xa8) = 0;
                    *(unsigned char *)(self + 0x107) = 0;
                } else {
                    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, self + 0xd4);
                    *(int *)(self + 0xa8) = _ZN4cstd4fdivEii(
                        (*(int *)(self + 0xa8) * -50) / 100, *(int *)(self + 0xd8));
                }
                {
                    int *pa = (int *)(self + 0xa4);
                    *pa += *(int *)(self + 0xd4) << 3;
                    *(int *)(self + 0xac) += *(int *)(self + 0xdc) << 3;
                    *(int *)(self + 0x98) = Vec3_HorzLen(pa);
                }
            }
            *(s16 *)(self + 0x8e) = *(s16 *)(self + 0x94);
            if (lim == 0)
                _ZN12WithMeshClsn15ClearLimMovFlagEv(clsn);
        }
        return 3;
    }

    /* not being eaten: clear the eater link + eat bits */
    *(int *)(self + 0xd0) = 0;
    *(int *)(self + 0xb0) &= ~0xe0000;
    *(unsigned char *)(self + 0x107) = 0;
    return 0;
}

}  /* extern "C" */
