//cpp
#include "types.h"
// @symbol _ZN9LakituBro13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "LakituBro.h"
#include "SharedFilePtr.h"

/* SetAnim and SetFile stay mangled because their real signatures carry
   Fix12<int> (wall 6az). Everything else the members' own headers declare.

   PREPARE STAYS MANGLED FOR A DIFFERENT REASON, and it is not a style choice:
   `mTextureSequence.Prepare(model, file)` compiles, but costs bytes, because
   the ROM does not pass a `this` here. The call is

       ldr r0,[pc,#0xfc] / ldr r1,[pc,#0x100]   the two SharedFilePtrs
       ldr r0,[r0,#4]    / ldr r1,[r1,#4]       the two loaded files
       bl  _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File
       mov r2,#0                                <- r2 written AFTER the call

   -- exactly two argument registers. All 23 call sites in the tree declare it
   with two parameters, and a mangled name does not encode staticness, so the
   symbol is equally the mangling of a STATIC member. TextureSequence.h
   declaring it non-static is what makes the member form unwritable anywhere.
   Its 0xc body is `ldr r12,[pc]; bx r12`, a pure tail jump that reproduces for
   any signature, so the definition file's `this` is unconstrained by the ROM
   and contradicted by every caller. Fixing that is a shared-header change with
   23 dependents and belongs in its own commit, not here. */
extern "C" void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(BMD_File& model, BTP_File& file);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, BCA_File* f, int a, Fix12i b, u32 c);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* thiz, BTP_File& f, int a, Fix12i b, u32 c);

extern char data_ov085_0213074c[];   /* SharedFilePtr; loaded file at +4 */
extern char data_ov085_02130744[];
extern char data_ov085_0213073c[];
extern int data_ov085_021307d0;
extern int data_ov085_021307e0;
extern char data_0209caa0[];

int LakituBro::InitResources()
{
  BMD_File* bmd = (BMD_File*)Model::LoadFile(*(SharedFilePtr*)data_ov085_0213074c);
  mModelAnim1.SetFile(bmd, 1, -1);
  Animation::LoadFile(*(SharedFilePtr*)data_ov085_02130744);
  TextureSequence::LoadFile(*(SharedFilePtr*)data_ov085_0213073c);
  mShadowModel1.InitCylinder();
  mShadowModel2.InitCylinder();
  _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
      **(BMD_File**)(data_ov085_0213074c + 4), **(BTP_File**)(data_ov085_0213073c + 4));
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
      &mModelAnim1, *(BCA_File**)(data_ov085_02130744 + 4), 0, 0x1000, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
      &mTextureSequence, **(BTP_File**)(data_ov085_0213073c + 4), 0, 0x1000, 0);
  mModelAnim1.speed = 0x1000;
  unk_2d0 = param1 & 0xff;
  if (unk_2d0 == 0xff)
    unk_2d0 = 0;
  switch (unk_2d0) {
  case 0:
    func_ov085_0212e728(((char*)this), &data_ov085_021307d0);
    break;
  case 1:
    {
      int v = *(int*)(data_0209caa0 + 8);
      if (v & 0x20000)
        return 0;
      if (v & 0x10000)
        *(int*)(data_0209caa0 + 8) = v & ~0x10000;
      unk_2dc = 1;
      if (!(*(int*)(data_0209caa0 + 8) & 0x80))
        func_ov085_0212e728(((char*)this), &data_ov085_02130790);
      else
        func_ov085_0212e728(((char*)this), &data_ov085_021307e0);
    }
    break;
  }
  return 1;
}
