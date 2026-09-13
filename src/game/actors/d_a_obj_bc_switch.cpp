//cpp
/* The blue-coin switch -- ov002/daObjBC_Switch_c.
 *
 * A GENUINE TRANSLATION UNIT, ENROLLED AND CANONICAL. It is the whole of the
 * cartridge's contiguous linker run .text 0x020f11b0..0x020f15fc, nine
 * functions and nothing else, and the production build links this object
 * rather than nine per-function ones. The filename is tools/tu_names.py's:
 * candidate_stem('daObjBC_Switch_c') is d_a_obj_bc_switch.
 *
 * THE CLASS IS NAMED FROM THE CARTRIDGE, not coined. ov002 0x0210b228 is a
 * __si_class_type_info whose _ZTS at 0x0210b234 reads exactly
 * `16daObjBC_Switch_c`, and whose one base, at subobject offset 0, is ov002
 * 0x021089ec -- _ZTI10dBgActor_c, whose own _ZTS reads `10dBgActor_c`. That is
 * why the header says `struct daObjBC_Switch_c : dBgActor_c`, and why the RTTI
 * below has ROM homes to be compared against at all. It used to be called
 * BlueCoinSwitch, which was a coined placeholder and matched nothing at any
 * address. Profile BLUE_COIN_SWITCH(10) in symbols/overlay_actors.md.
 *
 * IT IS A LEAF. A byte scan of every file under extracted/ for the LE word
 * 0x0210b228 finds exactly one logical hit -- file offset 0x5dc08 in both
 * ov002.bin and overlay_0002.bin, which is 0x020ad660 + 0x5dc08 = 0x0210b268 =
 * this class's own _ZTV - 4. No other class's __si_class_type_info names it as
 * a base, so nothing in the ROM derives from it.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor variants are the one group whose order the compiler picks rather
 * than source text; with the destructor inline (see below) it picks D1 then
 * D0, which is the ROM's order, and tubuild verify reports all nine functions
 * in the expected ROM-ascending section order.
 *
 * THE SIZE ARITHMETIC CLOSES, VIA THE BASE'S TAIL PADDING. The factory's
 * literal is 816 = 0x330. dBgActor_c's members END at 0x31e and its sizeof
 * rounds up to 0x320; include/dBgActor_c.h says so explicitly and says derived
 * classes do reuse that tail padding. This class does: pad_31e sits AT 0x31e,
 * inside the base's rounding slack, and the seven named members then span
 * 0x320..0x32e, with one byte of tail padding after -- 0x330 exactly. Reading
 * the rounded 0x320 as the first free offset would put mStopPosY at 0x324 and
 * the class at 0x334, which the size assert in the header would reject.
 *
 * NAMING THE CLASS PUTS ITS VAGUE-LINKAGE DATA IN THIS OBJECT, because the
 * class has NO key function. Its only virtual is the destructor and that is
 * defined inline in the class body, so there is no out-of-line virtual to
 * anchor the vtable to one object; mwcc therefore emits the vtable and the
 * RTTI here, as vague linkage, in every TU that needs them. That is the
 * opposite of the key-function case, and it is why these records need
 * licensing at all rather than simply belonging here.
 *
 * Eleven records come out, each licensed as deadstrip-data in the manifest and
 * word-compared by romdata_check against the cartridge, relocations applied,
 * before production isolation discards it: this class's own _ZTV/_ZTI/_ZTS at
 * ov002 0x0210b26c/0x0210b228/0x0210b234, dBgActor_c's _ZTI/_ZTS pair at ov002
 * 0x021089ec/0x02108a04, and the six arm9 records -- an _ZTI and an _ZTS each
 * for dActor_c, dBase_c and fBase_c. 3 + 2 + 6 = 11.
 *
 * THE VTABLE CLAIM IS SCOPED TO THE SLOTS, deliberately. A symbols.txt _ZTV
 * address is the ADDRESS POINT, eight bytes past the table's real start; the
 * {offset-to-top, _ZTI} header pair at 0x0210b264 reads {0, 0x0210b228} and is
 * word-compared by nothing, so the 32 slots are proved and those two words are
 * not. Diffed against _ZTV10dBgActor_c (ov002 0x0210ae38) the two tables are 32
 * words each and differ in exactly slots 0, 3, 6, 9, 16 and 17 --
 * InitResources, CleanupResources, Behavior, Render, D1 and D0, precisely the
 * six members the header declares.
 *
 * THE DESTRUCTOR IS INLINE IN THE CLASS BODY, AND MUST STAY THERE. Out of
 * line, objisolate called the object clean and the bytes matched 9/9 -- but
 * mwcc emitted D0 before D1 plus a homeless D2, and the ROM has D1 at
 * 0x020f11b0 then D0 at 0x020f11f4 and no D2 at all. Production isolation
 * places .text into the spanning delink in EMISSION order, so that shape
 * failed `tubuild.py linkcheck` even though objisolate passed it: objisolate
 * checks one object's relocations, not the order the linker will see. Moving
 * the body into the class declaration flips the emission to D1, D0, no D2.
 * The leaf proof above is what makes that safe -- nothing in the ROM derives
 * from this class, so no other TU's codegen can shift with it.
 *
 * NO PRAGMAS. None of the nine legacy sources carried one.
 *
 * deslop leftovers:
 * - dBgW_KcMbg::SetFile 6az: InitResources' by-value Fix12<int> scale makes
 *   the header method form size-DIFF.
 * - dBgActor_c::IsClsnInRange 6az: Behavior's two by-value Fix12<int>
 *   parameters; the header method form is refused by the bytes
 *   (include/dBgActor_c.h).
 * - Sound::ChangeMusicVolume 6az: Behavior passes Fix12<int> by value.
 * - func_020393c4: InitResources stores func_ov002_020f15b8 on
 *   mMeshCollider; dBgW.h has no setter.
 * - data_ov002_0210d6f4 CLPS handle: this TU claims .text only.
 * - InitResources 0x32a: named mCoinTimerSeed CSE's the field address.
 *   Keep (u16*)(c+0x300)[0x15] / ((int)c+0x32a).
 * - Event::SetBit stays mangled: no Event header.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x020f11b0  src/_ZN16daObjBC_Switch_cD1Ev.cpp
 *   [1] 0x020f11f4  src/_ZN16daObjBC_Switch_cD0Ev.cpp
 *   [2] 0x020f124c  src/_ZN16daObjBC_Switch_c16CleanupResourcesEv.cpp
 *   [3] 0x020f1290  src/_ZN16daObjBC_Switch_c6RenderEv.cpp
 *   [4] 0x020f12c8  src/_ZN16daObjBC_Switch_c8BehaviorEv.cpp
 *   [5] 0x020f1468  src/_ZN16daObjBC_Switch_c13InitResourcesEv.cpp
 *   [6] 0x020f1578  src/func_ov002_020f1578.c
 *   [7] 0x020f15b8  src/func_ov002_020f15b8.c
 *   [8] 0x020f15cc  src/daObjBC_Switch_c_classInit.c
 */

/* daObjBC_Switch_c.h FIRST: it pulls in dBgActor_c.h, which must reach
   common.h ahead of Model.h or the wrong Matrix4x3 spelling wins. */
#include "daObjBC_Switch_c.h"
#include "SharedFilePtr.h"
#include "decl_common.h"

extern "C" {
extern SharedFilePtr daObjBC_Switch_c_ClsnFile;
extern SharedFilePtr daObjBC_Switch_c_ModelFile;
extern u8 IsAreaShowing(s32 idx);
/* Sound::ChangeMusicVolume(u32, Fix12<int>) -- wall 6az on Behavior. */
extern s32 _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 a, s32 vol);
extern void _ZN5Event6SetBitEj(u32 bit);
extern u16 DecIfAbove0_Short(u16 *p);
extern void *_ZN8dActor_c15FindWithActorIDEjPS_(u32 id, void *prev);
/* dBgActor_c::IsClsnInRange(Fix12<int>, Fix12<int>) -- wall 6az on Behavior. */
extern s32 _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, s32 a, s32 b);
/* dBgW_KcMbg::SetFile -- wall 6az on InitResources. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, Matrix4x3 *mat, int scale,
    short angY, void *clps);
extern "C" void func_020393c4(void* p, void* v);
extern int func_ov002_020dd8b8(void* c);
extern void func_ov002_020f1578(daObjBC_Switch_c *c, char *arg);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjBC_Switch_c_classInit
/* Reconstructed source-style name: the ROM proves daObjBC_Switch_c and the
 * BC_SWITCH profile relationship; later EAD lineage supplies classInit.
 * Exact original SM64DS spelling is not preserved.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x020f15d0 loads 816 = 0x330 -- the class's own size -- into the header's
 * inline operator new; 0x020f15e0 calls dBgActor_c's C2 and the store at
 * 0x020f15ec lays down this class's vptr. The null check is the one `new`
 * itself emits. */
extern "C" daObjBC_Switch_c *daObjBC_Switch_c_classInit(void)
{
    return new daObjBC_Switch_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f15b8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f15b8(void* a, void* b, void* c) {
    func_ov002_020f1578((daObjBC_Switch_c *)b, (char*)c);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f1578
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f1578(daObjBC_Switch_c *c, char *arg){
    /* (int) on the comparison is load-bearing bool-widening. arg+0xc is the
       colliding actor's actorID (PLAYER is 0xbf); Player.h is out of scope. */
    int b = (int)(*(unsigned short*)(arg + 0xc) == 0xbf);
    if (b == 0) return;
    if (func_ov002_020dd8b8(arg) != 0)
        c->mPressed = 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjBC_Switch_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Loads the model and collision mesh, unpacks the spawn word, and computes how
 * far the switch will sink.
 *
 * param1 carries two fields: bits 0-3 are the event bit to set when the switch
 * bottoms out, bits 8-15 are the countdown seed. That seed is stored in TENTHS
 * -- it is multiplied by 10 here -- and a byte of 0 or 0xff means "use the
 * default 0xfa" instead.
 *
 * mStopPosY is just mPosY - 0x64000, so the switch always sinks the same distance
 * from wherever it was placed rather than to a fixed height.
 *
 * mAreaId is copied into mHomeAreaId before Behavior sets mAreaId to -1, which is
 * how the switch still knows which area it belongs to after it stops claiming
 * membership.
 *
 * THE ODD SPELLINGS ARE LOAD-BEARING and are kept exactly: `p` is hoisted out
 * of the branch, and the 0x32a write is reached once through
 * `(u16*)(c + 0x300)[0x15]` and once through a masked direct address. Named
 * mCoinTimerSeed CSE's the field address and size-DIFFs.
 */
/* The name comes from vtable slot identity, not from the image. */
int daObjBC_Switch_c::InitResources()
{
    char* c = (char*)this;
    void* bmd;
    void* kcl;
    int y;
    unsigned int x;
    unsigned short val;
    unsigned short* p;

    bmd = Model::LoadFile(daObjBC_Switch_c_ModelFile);
    mModel.SetFile((BMD_File *)bmd, 1, -1);

    mEventBit = (int)param1 & 0xf;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    kcl = dBgW_Kc::LoadFile(daObjBC_Switch_c_ClsnFile);
    /* MEASURED on ov002/daObjBC_Switch_c::InitResources: this one call has
       to keep the mangled spelling. Its third parameter is a by-value
       Fix12<int> -- wall 6az -- and materialising one costs stack traffic
       the ROM does not have. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)kcl, &mClsnMat, 0x199, mAngleY,
        &data_ov002_0210d6f4);

    func_020393c4(&mMeshCollider, (void*)&func_ov002_020f15b8);

    mPressed = 0;

    /* Keep p out of the mid-block so y colors r1 and c+0x300 colors r0,
       and so set_fa rematerializes add r0,r4,#0x300 after ldrh clobbers r0. */
    y = mPosY;
    mStopPosY = y - 0x64000;
    x = param1;
    mCoinTimerSeed = (x >> 8) & 0xff;
    mHomeAreaId = mAreaId;
    val = mCoinTimerSeed;
    p = (unsigned short*)(c + 0x300);

    /* ROM: if (val==0xff || val==0) store 0xfa; else *= 10 */
    if (val == 0xff)
        goto set_fa;
    if (val != 0)
        goto multiply;

set_fa:
    p = (unsigned short*)(c + 0x300);
    p[0x15] = 0xfa;
    goto done;

multiply:
    *(unsigned short*)((int)c + 0x32a) =
        *(unsigned short*)((int)c + 0x32a) * 0xa;

done:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjBC_Switch_c8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * The switch's whole life, and none of it runs until mPressed says it has been
 * pressed.
 *
 * Once pressed: mAreaId is set to -1 (the switch stops claiming its area, which
 * is why init stashed the original in mHomeAreaId), the music ducks, and mPosY
 * walks down 0x14000 a frame toward mStopPosY. On arrival it snaps exactly to
 * mStopPosY, sets its event bit, arms the countdown from mCoinTimerSeed, disables the
 * collider and poofs -- so the sinking animation and the "switch is now on"
 * moment are the same event.
 *
 * mCoinTimer is both timer and state: non-zero means running, and expiry parks it
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
s32 daObjBC_Switch_c::Behavior()
{
    u16 t;

    if (mPressed == 1) {
        mAreaId = -1;
        if (IsAreaShowing((s8)mHomeAreaId) == 0)
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x3f000);
    }
    if (mPressed == 1) {
        if (mPosY > mStopPosY) {
            mPosY = mPosY - 0x14000;
            if (mPosY <= mStopPosY) {
                mPosY = mStopPosY;
                _ZN5Event6SetBitEj(mEventBit);
                mCoinTimer = mCoinTimerSeed;
                mMeshCollider.Disable();
                PoofDust();
            }
        }
        if (mCoinTimer != 0) {
            if (DecIfAbove0_Short(&mCoinTimer) == 0) {
                mCoinTimer = 1;
                if (_ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x64cc) != 0)
                    KillAndTrackInDeathTable();
            } else {
                t = mCoinTimer;
                if (t == 0x2d)
                    mTickSound = 0;
                else if (t < 0x2d)
                    mTickSound = func_02012310(mTickSound, 0x39, 0);
                else
                    mTickSound = func_02012310(mTickSound, 0x38, 0);
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x40, 0xc999);
                if (_ZN8dActor_c15FindWithActorIDEjPS_(0x122, 0) == 0)
                    mCoinTimer = 1;
            }
        }
    }
    UpdateModelPosAndRotY();
    /* MEASURED on ov002/daObjBC_Switch_c::Behavior: the two by-value
       Fix12<int> parameters are wall 6az. */
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x1f4000, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjBC_Switch_c6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * Draws the switch only while it is still above its stop. Behavior sinks mPosY
 * toward mStopPosY once pressed, so this comparison IS the visibility rule --
 * the switch vanishes exactly when it bottoms out, with no separate flag.
 */
int daObjBC_Switch_c::Render()
{
    if (mPosY > mStopPosY)
        mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjBC_Switch_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Disables the collider if it is still enabled -- so a second cleanup is
 * harmless -- then releases the model and collision files.
 */
int daObjBC_Switch_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    daObjBC_Switch_c_ModelFile.Release();
    daObjBC_Switch_c_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjBC_Switch_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring the destructor is enough. With the
 * body inline in the class declaration mwcc emits exactly D1 and D0, in that
 * order, and no D2 -- which is the ROM's shape, so both variants are licensed
 * functions of this TU and neither needs a deadstrip policy row.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 *
 * Deleted here: this legacy file's own body was a real out-of-line
 * daObjBC_Switch_c::~daObjBC_Switch_c(), identical to the one the legacy D1
 * file carried -- the plain D0/D1 redefinition collapse, not the mwcc ICE
 * shape. Neither copy survives: the single definition now lives in the class
 * body in include/daObjBC_Switch_c.h, and the compiler emits both variants
 * from it, byte-identical to the ROM.
 */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjBC_Switch_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct daObjBC_Switch_c : dBgActor_c`: its own vptr, then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds no
 * member with a destructor of its own.
 */
/* The destructor body lives in the class declaration in
 * include/daObjBC_Switch_c.h. Out of line here, mwcc emitted D0 before D1;
 * the ROM has D1 at 0x020f11b0 then D0 at 0x020f11f4, and production
 * isolation places .text in emission order, so the out-of-line spelling
 * failed linkcheck even though objisolate called it clean. Inline in the
 * class body it emits D1, D0 and no D2. */
