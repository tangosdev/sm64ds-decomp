// @symbol _ZN4Door13InitResourcesEv
#include "Door.h"
// recovered name: Door::InitResources
/* recovered: renamed to Class_Method, vtable slot 0 */
/* Door::InitResources -- vtable slot 0, ov100 0x021455a0. Declared as an
 * override in include/Door.h, defined here as a free function taking the
 * object pointer explicitly, the same idiom the rest of the class uses and
 * the one src/_ZN7fBase_c13InitResourcesEv.cpp uses for fBase_c's own slot 0.
 *
 * FOLDED ONTO include/Door.h. This file used to include the generated flat
 * placeholder include/daDoor_c.h -- which restated dActor_c's fields inline
 * as pad_000[0x5c] + unk_05c/unk_060/... -- and reach the rest of the object
 * through raw `c + 0xNN` arithmetic. It now takes a `struct Door *` and names
 * every field through the C-mode branch of Door.h, which nests
 * `struct dActor_c` and `ModelAnim` rather than restating their offsets. What
 * that buys, beyond the names: 0x05c/0x060/0x064 are dActor_c's mPosX/Y/Z and
 * 0x08e its mAngleY, so the opening call is visibly "rotate my own position
 * about my own facing angle"; 0x0f0 is mModel.mat4x3 and +0x1c of the second
 * Model is its mat4x3 too, so the two matrix stores are the same store to two
 * models; and 0x008 is fBase_c's param1, the spawn parameter this door's
 * whole variant table is indexed by.
 *
 * BYTE-MATCHES under the pinned 2004/b56, 0x2fc for 0x2fc, with relocation
 * destinations checked. It did not until the param1 shift below was respelt;
 * the one-instruction gap this file used to carry (0x300 against the ROM's
 * 0x2fc, diverging at +0x0c) is described at that line together with the
 * lever that closed it.
 *
 * config/arm9/overlays/ov100/delinks.txt still carries no `complete` marker
 * for this range, so dsd keeps supplying it from the cartridge and the ROM
 * build does not yet compile this file. That is a layout question, separate
 * from the byte question this file now answers, and is left where it was.
 *
 * NOT RENAMED BUT WORTH RECORDING: the block near the end writes mPosX,
 * mPosY + 0xb4000 and mPosZ into 0x0a4/0x0a8/0x0ac. dActor_c.h names the
 * middle of those three `mVertSpeed`, which cannot be what a Door is storing
 * there -- three consecutive words taking a position triple say 0x0a4..0x0af
 * is a second Vector3 for at least this class. Left spelt as dActor_c has
 * it, because renaming a base field on one derived class's evidence is a
 * dActor_c change with 62-plus consumers, not a Door one. */
enum { false, true };

typedef struct { int x, y, z; } Vec3;

struct Entry {
    void *sfp;
    void *sfp2;
    signed char b8;
    signed char b9;
    short pad;
    int pad2;
};

extern int data_ov100_02148710;
extern Vec3 data_ov100_021487c0;
extern void _ZN7Vector3D1Ev(void);
extern void *data_ov100_021487b4;
extern void func_020731dc(void *a, void *b, void **node);
extern void Vec3_RotateYAndTranslate(void *d, void *s, short ang, void *t);
extern struct Entry data_ov100_02148204[];
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern unsigned char data_0209f2d8;
extern int data_ov002_0211094c;
extern void *func_02132894[];
extern void LoadKeyModels(int idx);
extern int data_ov089_02132c50;
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern int data_ov100_02148744;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int fix, unsigned int j);
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern struct Matrix4x3 data_020a0e68;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
extern void *_Znwj(unsigned int sz);
extern void *_ZN5ModelC1Ev(void *self);
extern unsigned char data_0209f250;
extern int data_0209f394[];
/* Returns int: _ZN4Door8BehaviorEv passes its result straight into the
   callback, and the ROM keeps r0 across the call. The value is unused here. */
extern int func_ov100_02145370(struct Door *self);
extern int data_ov100_02148914;
extern int data_ov100_021488b4;
extern void func_ov100_021453d8(struct Door *self, void *p, int a2);

int _ZN4Door13InitResourcesEv(struct Door *self)
{
    unsigned int idx;
    struct Entry *e;
    void *f;
    void *an;
    void *m;
    Vec3 tmp;
    int r4;
    int v;
    int b;
    int x;
    int y;
    int z;

    /* The spawn word arrives packed: this door's variant index is param1's
       high halfword, and every later read of param1 in this function is that
       index. Shifted down in place, once, before anything else.

       The volatile round-trip on the read is a matching crutch, not
       semantics: casting a prvalue to a cv-qualified scalar discards the
       qualifier, so `(u32)(volatile u32)x` and `x` are the same value and it
       emits no code of its own. tools/delaunder.py knows this idiom as
       CVCAST and re-tests every site of it automatically. What it buys is
       the addressing mode. Spelt plainly, both sides of this assignment are
       the same expression, 2004/b56 value-numbers them together and
       materialises the address once -- `add r2, r5, #8` / `ldr r1, [r2]` /
       `lsr` / `str r1, [r2]` -- one instruction longer than the ROM, which
       shifts every literal-pool load in the rest of the function. The ROM
       keeps the offset folded into both accesses: `ldr r1, [r5, #8]` /
       `lsr` / `str r1, [r5, #8]`. Any spelling that makes the two sides
       textually different reaches the folded form; this is the one the tree
       already has a name and a gate for. Measured: with the cast the candidate
       is 0x2fc and 0 of 191 words differ; without it 0x300, and over the
       shared prefix 156 of 192 differ. */
    self->base.param1 = (u32)(volatile u32)self->base.param1 >> 0x10;

    if (!(data_ov100_02148710 & 1)) {
        data_ov100_021487c0.x = 0x4b000;
        data_ov100_021487c0.y = 0;
        data_ov100_021487c0.z = 0;
        func_020731dc(&data_ov100_021487c0, (void *)_ZN7Vector3D1Ev, &data_ov100_021487b4);
        data_ov100_02148710 |= 1;
    }

    Vec3_RotateYAndTranslate(&self->base.mPosX, &self->base.mPosX,
                             self->base.mAngleY, &data_ov100_021487c0);

    idx = self->base.param1;
    e = &data_ov100_02148204[idx];
    f = _ZN5Model8LoadFileER13SharedFilePtr(e->sfp);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&self->mModel, f, 1, 1);

    b = data_0209f2d8;
    b = b == 0;
    if (b != false) {
        if (e->b8 > 0) {
            self->mKeyFile = &data_ov002_0211094c;
        } else if (e->b9 >= 0) {
            unsigned int t = self->base.param1;
            if (t >= 9 && t <= 0xd) {
                self->mKeyModelIdx = (signed char)(t - 8);
                LoadKeyModels(self->mKeyModelIdx + 1);
                self->mKeyFile = func_02132894[self->mKeyModelIdx + 1];
                if (self->base.param1 == 0xc)
                    self->mKeyModelIdx = 0;
            } else {
                self->mKeyFile = &data_ov089_02132c50;
            }
        }
        if (self->mKeyFile != 0)
            _ZN5Model8LoadFileER13SharedFilePtr(self->mKeyFile);
    }

    an = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov100_02148744);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModel, an, 0x40000000, 0x1000, 0);

    Vec3_Asr(&tmp, (Vec3 *)&self->base.mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, tmp.x, tmp.y, tmp.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, self->base.mAngleY);
    self->mModel.mat4x3 = data_020a0e68;

    if (e->sfp2 != 0) {
        m = _Znwj(0x50);
        if (m != 0)
            m = _ZN5ModelC1Ev(m);
        self->mKeyModel = m;
        f = _ZN5Model8LoadFileER13SharedFilePtr(e->sfp2);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(self->mKeyModel, f, 1, -1);
        self->mKeyModel->mat4x3 = data_020a0e68;
    }

    {
        int w;
        y = self->base.mPosY;
        z = self->base.mPosZ;
        x = self->base.mPosX;
        w = y + 0xb4000;
        self->base.unk_0a4 = x;
        self->base.mVertSpeed = w;
        {
            unsigned char bi = data_0209f250;
            self->base.unk_0ac = z;
            r4 = data_0209f394[bi];
        }
    }
    func_ov100_02145370(self);

    v = self->base.mScaleX;
    if (v < 0)
        v = -v;
    if (v > 0x4b000)
        goto big;
    v = self->base.mScaleY;
    if (v < 0)
        v = -v;
    if (v > 0x32000)
        goto big;
    v = self->base.mScaleZ;
    if (v < 0)
        v = -v;
    if (v > 0x1f4000)
        goto big;

    func_ov100_021453d8(self, &data_ov100_02148914, r4);
    goto done;
big:
    func_ov100_021453d8(self, &data_ov100_021488b4, r4);
done:
    return 1;
}
