//cpp
/* daObjSwdoor_c -- ov002, the shared base of the switch-operated shutters
 * (RTTI and vtable evidence in include/daObjSwdoor_c.h).
 *
 * Slots 0 (InitResources), 3 (CleanupResources) and 6 (Behavior) are pure
 * virtual here; Render (slot 9) is the one real slot this class fills. The two
 * leaves, daObjBSwdoor_c (ov014) and daObjCvShutter_c (ov021), fill the null
 * slots by calling func_ov002_020bad10, func_ov002_020baba8 and
 * func_ov002_020bac18 with their own file tables.
 *
 * The class declares no fields: 0x31e and 0x31f sit in dBgActor_c's tail
 * padding, and 0x320 and 0x321 are the leaves' mTimer and mEventBit.
 *
 * Known limits:
 * - The three helpers keep their ROM-address names: slots 0, 3 and 6 are pure
 *   virtual, and the leaves call the helpers directly.
 * - func_ov002_020bad10 and func_ov002_020bac18 reach 0x31e..0x321 through
 *   byte offsets, because this class has no fields there; the leaves own them.
 * - func_ov002_020bac18 keeps the LM/LMS/LMI launders; writing through a
 *   member changes what mwccarm CSEs.
 * - dBgW_KcMbg::SetFile stays mangled (Fix12<int> by value).
 * - func_020393d4 is a 4-byte store into the dBgW at +0x18; naming it belongs
 *   with dBgW in arm9.
 * - Event::GetBit stays mangled: there is no Event.h.
 * - Abstract class: no factory and no g_profile row.
 */

#pragma defer_codegen off

#include "daObjSwdoor_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
unsigned int _ZN5Event6GetBitEj(unsigned int bit);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_020393d4(dBgW_KcMbg *clsn, void *fn);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);

int func_ov002_020bad10(daObjSwdoor_c *self, ResourceDescriptor *descriptor);
int func_ov002_020bac18(daObjSwdoor_c *self);
int func_ov002_020baba8(daObjSwdoor_c *self, ResourceDescriptor *descriptor);
}

// @symbol _ZN13daObjSwdoor_cD0Ev
// @symbol _ZN13daObjSwdoor_cD1Ev
/* ROM ordinals 0 and 1 -- ov002 0x020bab0c (D0, 0x58) and 0x020bab64 (D1,
 * 0x44). No source here: both destructor variants come from the ONE
 * inline body in include/daObjSwdoor_c.h, which the class's descendants need
 * visible to inline its vptr store.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, which no deferred form reaches without a
 * D2 the image does not contain. A delete-expression for D0 because
 * dBgActor_c declares Kill, a key function reachable from this class. */

#ifdef _MSC_VER
/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" daObjSwdoor_c *_ZN13daObjSwdoor_cD0Ev(daObjSwdoor_c *thiz)
{
    thiz->daObjSwdoor_c::~daObjSwdoor_c();          /* direct member/base teardown */
    daObjSwdoor_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjSwdoor_c_EmitDeletingDestructor(daObjSwdoor_c *p)
{
    delete p;
}
#endif

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjSwdoor_c_EmitDestructor(daObjSwdoor_c *p)
{
    p->~daObjSwdoor_c();
}

/* func_ov002_020baba8, 0x020baba8, size 0x48.
 * Teardown both leaves call: disable the collider if enabled, release the
 * descriptor's two files. */
// @symbol func_ov002_020baba8
extern "C" {
int func_ov002_020baba8(daObjSwdoor_c *self, ResourceDescriptor *descriptor)
{
    if (self->mMeshCollider.IsEnabled())
        self->mMeshCollider.Disable();
    descriptor->model->Release();
    descriptor->collision->Release();
    return 1;
}
}

/* Vtable slot 9, ov002 0x020babf0. Key function. */
// @symbol _ZN13daObjSwdoor_c6RenderEv
s32 daObjSwdoor_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* func_ov002_020bac18, 0x020bac18, size 0xf8.
 * Per-frame shutter the leaves install in the null Behavior slot. The state
 * byte at 0x31f runs 0, 1, 2:
 *   0  wait for the event bit and for the timer to run out, then arm a
 *      0x40-frame timer, clear the 0x1 bit of mFlags and play sound 0x3f;
 *   1  turn mAngleY by 0x100 a frame (direction from 0x31e) until the timer
 *      runs out, then set the 0x1 bit of mFlags again;
 *   2  done. */
// @symbol func_ov002_020bac18
extern "C" {

#define LM(p) ((u8*)(int)(p))
#define LMS(p) ((short*)(int)(p))
#define LMI(p) ((int*)(int)(p))

int func_ov002_020bac18(daObjSwdoor_c *self)
{
    char *c = (char *)self;
    u8 state = *(u8*)(c + 0x31f);

    switch (state) {
    case 0:
        if (_ZN5Event6GetBitEj(*(u8*)(c+0x321)) != 0) {
            if (DecIfAbove0_Byte((u8*)(c+0x320)) == 0) {
                *(u8*)(c+0x320) = 0x40;
                (*LM(c+0x31f))++;
                *LMI(&self->mFlags) &= ~1;
                Sound::PlayBank3(0x3f, *(Vector3 *)&self->mCamSpacePosX);
            }
        }
        break;
    case 1:
        if (DecIfAbove0_Byte((u8*)(c+0x320)) != 0) {
            if (*(u8*)(c+0x31e) != 0) {
                *LMS(&self->mAngleY) += 0x100;
            } else {
                *LMS(&self->mAngleY) -= 0x100;
            }
        } else {
            (*LM(c+0x31f))++;
            *LMI(&self->mFlags) |= 1;
        }
        break;
    case 2:
        break;
    }

    self->UpdateModelPosAndRotY();
    return 1;
}
}

/* func_ov002_020bad10, 0x020bad10, size 0xc0.
 * Resource setup both leaves call. The descriptor's model file goes to
 * Model::LoadFile, its collision file to dBgW_Kc::LoadFile, and its CLPS block
 * into SetFile; then the stock UpdatePosAndAngs hook and Enable. The four
 * bytes at 0x31e..0x321 are set last: bit 0 of param1 (the turn direction
 * func_ov002_020bac18 reads), the event bit from bits 1..5 of param1, the
 * state (0) and the start-up timer (5). */
// @symbol func_ov002_020bad10
extern "C" {
int func_ov002_020bad10(daObjSwdoor_c *self, ResourceDescriptor *descriptor)
{
    self->mModel.SetFile((BMD_File *)Model::LoadFile(*descriptor->model), 1, -1);
    self->UpdateModelPosAndRotY();
    self->UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &self->mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*descriptor->collision),
        &self->mClsnMat, 0x1000, self->mAngleY, descriptor->clps);
    func_020393d4(&self->mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);
    self->mMeshCollider.Enable(self);
    *(unsigned char *)((char *)self + 0x31e) = self->param1 & 1;
    *(unsigned char *)((char *)self + 0x321) = (self->param1 >> 1) & 0x1f;
    *(unsigned char *)((char *)self + 0x31f) = 0;
    *(unsigned char *)((char *)self + 0x320) = 5;
    return 1;
}
}
