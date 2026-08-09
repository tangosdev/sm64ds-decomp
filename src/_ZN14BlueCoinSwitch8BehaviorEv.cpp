//cpp
// @symbol _ZN14BlueCoinSwitch8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * The switch's whole life, and none of it runs until unk_32c says it has been
 * pressed.
 *
 * Once pressed: mAreaId is set to -1 (the switch stops claiming its area, which
 * is why init stashed the original in unk_32e), the music ducks, and mPosY
 * walks down 0x14000 a frame toward unk_320. On arrival it snaps exactly to
 * unk_320, sets its event bit, arms the countdown from unk_32a, disables the
 * collider and poofs -- so the sinking animation and the "switch is now on"
 * moment are the same event.
 *
 * unk_328 is both timer and state: non-zero means running, and expiry parks it
 * at 1 rather than 0 so it stays latched. Two things end it -- the count
 * reaching zero, or every blue coin (actor 0x122) being gone, which is checked
 * each frame and short-circuits the timer.
 *
 * The 0x2d boundary picks which sound parameter is stepped, so the cue changes
 * with 45 frames left rather than at the end.
 *
 * Position and collision are updated regardless, and the collider only when the
 * player is within 0x1f4000.
 */
#include "BlueCoinSwitch.h"
// @symbol _ZN14BlueCoinSwitch8BehaviorEv
// recovered name: daObjBC_Switch_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method */
/* daObjBC_Switch_c::Behavior - recovered from vtable slot identity */
/* These are ROM symbols spelled by their exact final names, so this .cpp must
   NOT mangle them. Without the extern "C" a bare `extern` on an already-mangled
   name mangles it a SECOND time -- _ZN5Event6SetBitEj becomes
   _Z18_ZN5Event6SetBitEjj, which exists nowhere. build_pin still passes,
   because match.py compares relocated words as wildcards; only eligible.py and
   check_references see it. This file was a .c and inherited C linkage for free;
   converting it is what put the declarations at risk. */
extern "C" {
extern u8 IsAreaShowing(s32 idx);
extern s32 _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 a, s32 vol);
extern void _ZN5Event6SetBitEj(u32 bit);
extern void _ZN5Actor8PoofDustEv(void *self);
extern u16 DecIfAbove0_Short(u16 *p);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
extern void *_ZN5Actor15FindWithActorIDEjPS_(u32 id, void *prev);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern s32 _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, s32 a, s32 b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);

#include "decl_common.h"
}

s32 BlueCoinSwitch::Behavior()
{
    char *c = (char *)this;
    u16 t;

    if (unk_32c == 1) {
        mAreaId = -1;
        if (IsAreaShowing((s8)unk_32e) == 0)
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x3f000);
    }
    if (unk_32c == 1) {
        if (mPosY > unk_320) {
            mPosY = mPosY - 0x14000;
            if (mPosY <= unk_320) {
                mPosY = unk_320;
                _ZN5Event6SetBitEj(unk_32d);
                unk_328 = unk_32a;
                _ZN16MeshColliderBase7DisableEv(&mMeshCollider);
                _ZN5Actor8PoofDustEv(c);
            }
        }
        if (unk_328 != 0) {
            if (DecIfAbove0_Short(&unk_328) == 0) {
                unk_328 = 1;
                if (_ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x64cc) != 0)
                    _ZN5Actor24KillAndTrackInDeathTableEv(c);
            } else {
                t = unk_328;
                if (t == 0x2d)
                    unk_324 = 0;
                else if (t < 0x2d)
                    unk_324 = func_02012310(unk_324, 0x39, 0);
                else
                    unk_324 = func_02012310(unk_324, 0x38, 0);
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x40, 0xc999);
                if (_ZN5Actor15FindWithActorIDEjPS_(0x122, 0) == 0)
                    unk_328 = 1;
            }
        }
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x1f4000, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
