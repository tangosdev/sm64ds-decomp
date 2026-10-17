//cpp
/* Drop-shadow models (arm9 .text 0x02015d38..0x0201609c): dExtShadowModel_c, the
 * ModelBase sibling that draws an actor's ground shadow from a shared
 * cylinder or cuboid BMD_File. Every live instance sits on a global
 * intrusive doubly-linked list (head data_0209cef4, freeze flag
 * data_0209ceec): InitModel links in, the destructor unlinks, RenderAll
 * walks the list and CleanAll empties it.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending.
 *
 * Vtable _ZTV17dExtShadowModel_c at 0x0208e868: [0] D1 0x02015ff8, [1] D0
 * 0x02015f80, [2] DoSetFile 0x02015ef4. The cartridge does carry _ZTI17dExtShadowModel_c/_ZTS17dExtShadowModel_c
 * (0x0208e770/0x0208e888), but no data section is enrolled for this TU to
 * own them, so this TU compiles with RTTI off (see below), and no enrolled D2 or C2;
 * those variants are licensed compiler-only in the manifest.
 *
 * leftovers:
 * - RTTI stays off: the cartridge's _ZTI/_ZTS records sit in data this TU
 *   does not own.
 * - The two BMD_File externs use data_ov000_ spellings: arm9's own
 *   symbols.txt rows for 0x020ad524/0x020ad560 name nothing the link
 *   defines.
 * - func_02016e70, func_0203c178, MulMat4x3Mat4x3, func_02046120,
 *   func_02046088 and the list-head/render-matrix globals have no
 *   declaring header.
 * - InitModel keeps its C-front-end body: the real signature carries
 *   three Fix12<int> by value and the member form homes them to the
 *   stack (wall 6az); the (char*) launders inside are the scalar
 *   spellings that keep the bytes.
 * - *(u32 *)(&mat->flags) and the +0x30 byte stride in DoSetFile's
 *   material loop keep the ROM's materialized addresses.
 * - CleanAll re-reads the list head for each store the way the ROM does.
 */

#include "dExtShadowModel_c.h"

/* The cartridge's _ZTI/_ZTS records for dExtShadowModel_c (0x0208e770/0x0208e888)
 * sit in data this TU does not own, so records emitted here would have no
 * configured home. Compiling with RTTI off emits none; the vtable preamble's
 * typeinfo word deadstrips with the rest of the data sections. */
#pragma RTTI off

extern "C" {
extern dExtShadowModel_c *data_0209cef4;
extern u8 data_0209ceec;
extern Matrix4x3 data_020a0e68;      /* shared render matrix scratch */
extern Matrix4x3 data_0209b3ec;      /* the camera-space matrix Model::Render uses */
/* The two model files live in the shared overlay-0/1 region: arm9's own
 * symbols.txt rows for 0x020ad524/0x020ad560 sit past .bss and name nothing
 * the link defines, so the ov000 spelling is the resolvable one. */
extern BMD_File data_ov000_020ad524;
extern BMD_File data_ov000_020ad560;
ModelComponents *func_02016e70(BMD_File *file);
void func_0203c178(Matrix4x3 *m, Fix12i sx, Fix12i sy, Fix12i sz);
void MulMat4x3Mat4x3(const Matrix4x3 *m1, const Matrix4x3 *m0, Matrix4x3 *mF);
void func_02046120(ModelComponents *data, u32 opacity);
void func_02046088(ModelComponents *data, u32 opacity, int f);
}

// @symbol _ZN17dExtShadowModel_cC1Ev
/* An empty body; the three null stores after the vptr are exactly the member
 * init list -- mat at +0xc and the live-list links prev/next at +0x20/+0x24,
 * the same links ~dExtShadowModel_c unlinks. The base call and vptr store are what
 * `dExtShadowModel_c : ModelBase` with a declared-not-defined base constructor
 * already means. */
dExtShadowModel_c::dExtShadowModel_c() : mat(0), prev(0), next(0) {}

// @symbol _ZN17dExtShadowModel_cD1Ev
/* Only the unlink is written by hand: the vtable store at the top and the
 * ModelBase subobject call at the bottom are what `struct dExtShadowModel_c :
 * ModelBase` and `virtual ~dExtShadowModel_c()` already mean, and the compiler
 * emits them around the body. The list is singly-headed, so removing the
 * first node is the case that needs the head compared against `this`; every
 * other node is reached through its predecessor. The unlink is also the
 * layout evidence for the two pointers: it is what pins prev at 0x20 and
 * next at 0x24. D0 is generated from the same body plus the class
 * operator delete -- the model family deallocates through
 * Memory::operator_delete2, which is why ModelBase carries that member. */
dExtShadowModel_c::~dExtShadowModel_c()
{
    if (prev)
        prev->next = next;
    else if (data_0209cef4 == this)
        data_0209cef4 = next;

    if (next)
        next->prev = prev;

    prev = 0;
    next = 0;
}

// @symbol _ZN17dExtShadowModel_c9DoSetFileEPcii
/* Overrides the base slot, so it takes (char *, int, int) like its
   siblings; the polygon-ID argument is simply never used -- a shadow has
   no polygon ID to set. */
int dExtShadowModel_c::DoSetFile(char *file, int a, int b)
{
    data = func_02016e70((BMD_File *)file);
    if (data == 0)
        return 0;
    data->UpdateVertsUsingBones();
    if (a != 0) {
        BMD_File *mf = data->modelFile;
        u32 n = mf->numMaterials;
        BMD_Material *mat = data->materials;
        u32 i;
        for (i = 0; i < n; i++) {
            /* (long long)(int) launder: keep the ROM's materialized
               address for the read-modify-write. */
            *(u32 *)(&mat->flags) |= 0x8000;
            mat = (BMD_Material *)((char *)mat + 0x30);
        }
    }
    return 1;
}

// @symbol _ZN17dExtShadowModel_c10InitCuboidEv
int dExtShadowModel_c::InitCuboid()
{
    return SetFile(&data_ov000_020ad524, 1, -1);
}

// @symbol _ZN17dExtShadowModel_c12InitCylinderEv
int dExtShadowModel_c::InitCylinder()
{
    return SetFile(&data_ov000_020ad560, 1, -1);
}

#pragma cplusplus off
/* The mangled name is dExtShadowModel_c::InitModel(Matrix4x3 *, Fix12<int>,
 * Fix12<int>, Fix12<int>, u32) and include/dExtShadowModel_c.h declares exactly
 * that. The DEFINITION stays in the C front end because of wall 6az in
 * notes/mwccarm-codegen.md: CW homes class-typed by-value parameters to the
 * stack, so a real method body with the true Fix12<int> signature comes out
 * 0x14 bytes bigger than the ROM. Scalar args here keep the bytes.
 */
// @symbol _ZN17dExtShadowModel_c9InitModelEP9Matrix4x35Fix12IiES3_S3_j
void _ZN17dExtShadowModel_c9InitModelEP9Matrix4x35Fix12IiES3_S3_j(struct dExtShadowModel_c *self, int a1, int a2, int a3, int a4, unsigned char a5) {
  *(int*)((char*)&self->mat) = a1;
  *(int*)((char*)&self->scale.x) = a2;
  *(int*)((char*)&self->scale.y) = a3;
  *(int*)((char*)&self->scale.z) = a4;
  *(unsigned char*)((char*)&self->opacity) = a5;
  if(data_0209ceec) return;
  *(void**)((char*)&self->next) = *(void* volatile*)&data_0209cef4;
  {
    void* head = (void*)data_0209cef4;
    if(head) *(void**)((char*)head+0x20) = ((void*)self);
  }
  data_0209cef4 = self;
}
#pragma cplusplus on

// @symbol _ZN17dExtShadowModel_c8CleanAllEv
void dExtShadowModel_c::CleanAll()
{
    if (data_0209cef4) {
        do {
            dExtShadowModel_c *nx = data_0209cef4->next;
            if (nx)
                nx->prev = 0;
            /* the ROM re-reads the head for this store instead of keeping
               it in a register */
            data_0209cef4->next = 0;
            data_0209cef4 = nx;
        } while (data_0209cef4);
    }
    data_0209ceec = 0;
}

// @symbol _ZN17dExtShadowModel_c9RenderAllEv
void dExtShadowModel_c::RenderAll()
{
    dExtShadowModel_c *node = data_0209cef4;
    if (node) {
        do {
            ModelComponents *model = node->data;
            func_0203c178(&data_020a0e68, node->scale.x, node->scale.y, node->scale.z);
            MulMat4x3Mat4x3(&data_020a0e68, node->mat, &data_020a0e68);
            data_020a0e68.t.y += 0x2000;
            MulMat4x3Mat4x3(&data_020a0e68, &data_0209b3ec, &data_020a0e68);
            func_02046120(model, node->opacity);
            model->Render(&data_020a0e68, 0);
            func_02046088(model, node->opacity, 1);
            model->Render(&data_020a0e68, 0);
            node = node->next;
        } while (node);
    }
    data_0209ceec = 1;
}
