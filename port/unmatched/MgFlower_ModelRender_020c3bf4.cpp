/* HOST COPY of src/func_ov006_020c3bf4.cpp -- the per-frame model draw of
 * dScMgFlower_c's 3D sub-object (the flower stem and the Yoshi that holds it).
 * Run mg5, lane Y3D.
 *
 * WHAT IT FIXES, and it is the LOCAL SHADOW CLASS problem
 * port/unmatched/Door_Render.cpp already names, one class up and one call
 * site over. hal/cxxname_bridge.cpp fills the host model vtables in MSVC
 * order, where the destructor costs ONE slot and Itanium's costs two:
 *
 *   _ZTV5Model      dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4
 *   _ZTV9ModelAnim  dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4,
 *                   Virtual18 5
 *
 * src/func_ov006_020c3bf4.cpp models the object at +0xd18 with a local
 * `struct Obj` of six pure virtuals and calls `f5(0)`, which is the ROM's own
 * numbering: BYTE +0x14 of an Itanium vtable is the sixth word, and on both
 * model classes that word is Render. Read straight off the overlay at base
 * 0x020bfec0:
 *
 *   020c3ca4  ldr  ip,[pc,#0x50]        ; = 0x00000d18
 *   020c3ca8  mov  r1,#0                ; scale = NULL
 *   020c3cac  add  r0,r4,ip
 *   020c3cb0  ldr  r2,[r0]              ; vptr
 *   020c3cb4  ldr  r2,[r2,#0x14]        ; ROM slot 5 -> ModelAnim::Render
 *   020c3cb8  blx  r2
 *
 * On the host that same byte offset is the SIXTH MSVC word, which on
 * ModelAnim is Virtual18(u32 mat, const Vector3 *scale). The shadow class
 * passes one argument, so mat arrives 0, and ModelAnim::Virtual18's first
 * statement is Virtual10(*(Matrix4x3 *)mat) -- a matrix reference at address
 * zero. Model::Virtual10's `movups xmm0,[eax]` is where it lands.
 *
 * MEASURED, scene 390, 300 frames, this tree:
 *
 *   [quarantine] actor 307FA008 id 390 faulted (code c0000005 +0007a78c)
 *                -- FROZEN, frame continues
 *   flw_render -> func_ov006_0212aacc+0x11e -> func_ov006_020c3bf4+0xc7
 *     -> ModelAnim::Virtual18 -> ModelAnim::Virtual10 -> Model::Virtual10+0xc
 *   crash.txt eax 00000000, access 00000000 at 00000000
 *
 * The quarantine freezes the whole scene actor on that first frame, so the
 * slot census reads init 1, behavior 0, RENDER 1 over 300 frames and the run
 * still exits 0 and prints "clean". The 2D background survives because
 * InitResources had already programmed it; nothing 3D is ever submitted
 * (SM64DS_PPU_AUDIT: POLYGONS 0 and BEGIN_VTXS 0 on all 300 frames). That is
 * the player-visible report this lane was opened on -- "no flower and no
 * yoshi model" -- and Tango's own flight recorder carries the identical
 * quarantine line at his build's +00085cac.
 *
 * WHY THE CALL IS SPELLED AS A METHOD RATHER THAN AS A SLOT INDEX. The line
 * above it fills the model's OWN Matrix4x3: +0xd34 is +0xd18 + 0x1c, which is
 * Model::mat4x3, and Render is the virtual that consumes it. Writing the
 * matrix and then calling Render(NULL) is coherent; writing it and then
 * handing Virtual18 a null matrix is not, which is the second, independent
 * reading that says slot 5 is Render here.
 *
 * The dispatch stays VIRTUAL rather than qualified. The ROM's is
 * (ldr vptr / ldr [vptr,#0x14] / blx), the object is only known to be in the
 * ModelAnim family, and a qualified call would silently devirtualise a
 * subclass override. Door_Render.cpp qualifies because its object is exactly
 * a ModelAnim by construction; this one is not established that way.
 *
 * WHAT CHANGED FROM src, so it can be checked line by line: the local
 * `struct Obj` shadow and its `f5(0)` call are replaced by a virtual
 * ModelAnim::Render(0). Every other statement, constant, offset and loop
 * bound is the matched source's, transcribed unchanged -- including the raw
 * store of the packed light vector to 0x040004c8 and the 0x16-iteration petal
 * pass with its `c += 0x98` stride.
 */
#include "common.h"
#include "Model.h"
#include "ModelAnim.h"

extern "C" {
extern void func_0203cd80(struct Vector3 *v, int a);
extern void func_0203ccd4(struct Vector3 *v, short angle);
extern void func_02016a14(void *p, int a);
extern void func_02016a04(void *p, int a);
extern void func_ov006_020c35a8(char *c);
extern int data_ov006_0213aee8[12];
void func_ov006_020c3bf4(void *self);
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's).
void func_ov006_020c3bf4(void *self)
{
    char *c = (char *)self;
    struct Vector3 v;
    unsigned int packed;
    int i;

    v.x = 0;
    v.y = 0;
    v.z = 0xfffff008;
    func_0203cd80(&v, 0);
    func_0203ccd4(&v, 0x600);

    packed = (((short)v.x >> 3) & 0x3ff)
           | ((((short)v.y >> 3) & 0x3ff) << 10)
           | ((((short)v.z >> 3) & 0x3ff) << 20);
    *(unsigned int *)0x40004c8 = packed;

    func_02016a14(c + 0xd18, 0x2bff);
    func_02016a04(c + 0xd18, 0x1211);

    /* Model::mat4x3, +0x1c into the model at +0xd18. The ROM copies it as one
       flat twelve-word ldm/stm run, which is what the local view spells. */
    {
        struct Mtx12w { int m[12]; };
        *(struct Mtx12w *)(c + 0xd34) = *(const struct Mtx12w *)data_ov006_0213aee8;
    }

    ((ModelAnim *)(c + 0xd18))->Render(0);

    i = 0;
    {
        char *p = c + 8;
        for (; i < 0x16; i++) {
            if (*(int *)(c + 0x48) != 0)
                func_ov006_020c35a8(p);
            c += 0x98;
            p += 0x98;
        }
    }
}
