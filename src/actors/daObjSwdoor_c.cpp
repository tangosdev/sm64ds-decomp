//cpp
/* The switch-operated shutters' shared base -- ov002/daObjSwdoor_c.
 * Abstract in slots 0 (InitResources), 3 (CleanupResources) and 6
 * (Behavior). daObjBSwdoor_c (ov014) and daObjCvShutter_c (ov021) call
 * func_ov002_020bad10 / 020baba8 / 020bac18 with their own file tables.
 * Render (slot 9) is the one real slot this class fills. No fields here:
 * 0x31e/0x31f sit in dBgActor_c's tail padding; 0x320/0x321 are the
 * leaves' mTimer / mEventBit.
 *
 * deslop
 * Leftover: func_ov002_020bad10 / 020baba8 / 020bac18 keep ROM address
 *   names (slots 0, 3 and 6 are pure virtual; the two leaves call these).
 * Leftover: dBgW_KcMbg::SetFile stays mangled (Fix12<int> by value, wall 6az).
 * Leftover: func_020393d4 is a 4-byte store into dBgW+0x18. Naming belongs
 *   with dBgW in arm9.
 * Leftover: 020bad10 writes 0x31e/0x31f/0x320/0x321 through offsets -- this
 *   class has no fields; the leaves own the tail.
 * Leftover: 020bac18 keeps the LM/LMS/LMI launders and raw offsets --
 *   writing through a member changes what mwccarm CSEs.
 * Leftover: Event::GetBit stays mangled (no Event.h surface).
 * Leftover: abstract class -- no factory, no g_profile row (S14).
 */

#include "daObjSwdoor_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

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
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int bankId, const Vector3 *pos);
void func_020393d4(dBgW_KcMbg *clsn, void *fn);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);

int func_ov002_020bad10(daObjSwdoor_c *self, ResourceDescriptor *descriptor);
int func_ov002_020bac18(daObjSwdoor_c *self);
int func_ov002_020baba8(daObjSwdoor_c *self, ResourceDescriptor *descriptor);
}

/* ROM ordinal 3 -- func_ov002_020bad10, 0x020bad10, size 0xc0.
 * Shared resource setup both leaves call. Slot 0 is Model::LoadFile, slot 1
 * is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then the stock
 * UpdatePosAndAngs hook, Enable, and the four tail bytes out of param1. */
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
    *(unsigned char *)((char *)self + 0x31e) = *(int *)((char *)self + 8) & 1;
    *(unsigned char *)((char *)self + 0x321) = (*(unsigned int *)((char *)self + 8) >> 1) & 0x1f;
    *(unsigned char *)((char *)self + 0x31f) = 0;
    *(unsigned char *)((char *)self + 0x320) = 5;
    return 1;
}
}

/* ROM ordinal 2 -- func_ov002_020bac18, 0x020bac18, size 0xf8.
 * Per-frame shutter the leaves install in the null Behavior slot. */
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
                *LMI(c+0xb0) &= ~1;
                _ZN5Sound9PlayBank3EjRK7Vector3(0x3f, (const Vector3*)(c+0x74));
            }
        }
        break;
    case 1:
        if (DecIfAbove0_Byte((u8*)(c+0x320)) != 0) {
            if (*(u8*)(c+0x31e) != 0) {
                *LMS(c+0x8e) += 0x100;
            } else {
                *LMS(c+0x8e) -= 0x100;
            }
        } else {
            (*LM(c+0x31f))++;
            *LMI(c+0xb0) |= 1;
        }
        break;
    case 2:
        break;
    }

    self->UpdateModelPosAndRotY();
    return 1;
}
}

/* ROM ordinal 1 -- vtable slot 9, ov002 0x020babf0. Key function. */
// @symbol _ZN13daObjSwdoor_c6RenderEv
s32 daObjSwdoor_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 0 -- func_ov002_020baba8, 0x020baba8, size 0x48.
 * Teardown half both leaves call. */
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
