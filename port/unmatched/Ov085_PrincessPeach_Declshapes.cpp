/* HOST COPY of src/_ZN13PrincessPeach21UpdateGroundCollisionEP10dBgCh_Actr.cpp
 * -- one of PRINCESS_PEACH's (actor 186, ov085) per-frame helpers. It is not
 * a behaviour change and not an ABI question: it is a MEASURED MSVC
 * FRONT-END REFUSAL that the matched source cannot be edited out of, because
 * src/ is byte-verified and this port never edits it.
 *
 * ---- _ZN13PrincessPeach12UpdateLookAtEv IS NOT HOST-COPIED ANY MORE ----------
 *
 * Its C2733 refusal is gone from the synced src: the matched TU defines the
 * real member PrincessPeach::UpdateLookAt and compiles clean under the port's
 * own compile line (run linkfull lane HGFRONT1). What kept it out after that
 * was port/faces_sync.txt's FORWARD row for the member, which defined the
 * same member and called the body that stood here. That row is a REVERSE row
 * now and src/_ZN13PrincessPeach12UpdateLookAtEv.cpp is on
 * port/slice_w24_faceflip.txt. Its one ROM caller, `from:0x0212a53c
 * kind:arm_call to:0x02129dbc` in config/arm9/overlays/ov085/relocs.txt, is
 * inside PrincessPeach::Behavior, which calls the member directly.
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
 * hostgen.py's FALLS_OFF_RETURN table is the reclaim route for this body.
 */

// PORT_HOST_ABI: MSVC front-end refusal in the matched TU (C4716, an int body
// falling off the end). Behaviour unchanged.

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
