/* HOST COPY of src/_ZN9LakituBro13InitResourcesEv.cpp -- one of the TextureSequence::Prepare callers.
 *
 * BYTE-IDENTICAL to the byte-locked source except for the Prepare extern
 * declaration (given its real third parameter) and each Prepare CALL (given its
 * real third argument). Produced by copying the source verbatim and applying
 * exact string edits, so it cannot carry a transcription error.
 *
 * THE CALLING-CONVENTION SEAM:
 *
 * TextureSequence::Prepare's ROM body is a 0xc tail-call veneer into
 * func_02046d50, whose real definition (src/func_02046d50.c) is
 *     void func_02046d50(void* arg, struct Tbl* t)
 * -- TWO arguments: a name-resolution context and a table it dereferences. At
 * every Prepare call the ROM sets exactly r0 and r1 (proven by disasm; e.g.
 * func_ov002_020e5948 0x020e60f4, LakituBro 0x0212ec44, func_ov091 0x02133300),
 * and func_02046d50 uses r0 as `arg` and r1 as `t`.
 *
 * The host wrapper (hal/player_bridges.cpp) is a THREE-parameter thiscall face:
 *     Prepare(void* self, void* bmd, void* btp)
 *       -> ((TextureSequence*)self)->Prepare(*bmd, *btp)
 *       -> func_02046d50(self, bmd, btp)      // btp ignored by func_02046d50
 * so it needs self = r0 = the caller's first value, bmd = r1 = the caller's
 * SECOND value. The matched C passes only TWO arguments, so the wrapper reads a
 * third (btp) from an unwritten stack slot and DEREFERENCES it (`*(BTP_File*)btp`)
 * -> reads garbage / faults, before func_02046d50 (which never uses it) runs.
 *
 * THE FIX gives the call its third argument: the caller's own second value
 * again (a known-valid BTP_File pointer). func_02046d50 ignores it; the wrapper's
 * deref is now of a real object. Nothing else changes.
 *
 * src/_ZN9LakituBro13InitResourcesEv.cpp is dropped from slice_gate18.txt in favour of this file; the byte-locked source
 * is unchanged.
 */
//cpp
#include <cstdio>
#include <cstdlib>
#include "types.h"
// @symbol _ZN9LakituBro13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "LakituBro.h"
struct SharedFilePtr { int f0; void* f4; };
struct BMD_File; struct BTP_File; struct BCA_File;
struct ModelBase {};
struct ModelAnim {};
struct ShadowModel {};
struct TextureSequence {};

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr& f);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase* thiz, BMD_File* f, int a, int b);
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr& f);
extern "C" void _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr& f);
extern "C" void _ZN11ShadowModel12InitCylinderEv(ShadowModel* thiz);
extern "C" void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(BMD_File& a, BTP_File& b, BTP_File& c);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim* thiz, BCA_File* f, int a, Fix12i b, u32 c);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(TextureSequence* thiz, BTP_File& f, int a, Fix12i b, u32 c);

extern SharedFilePtr data_ov085_0213074c;
extern SharedFilePtr data_ov085_02130744;
extern SharedFilePtr data_ov085_0213073c;
extern int data_ov085_021307d0;
extern int data_ov085_021307e0;
extern char data_0209caa0[];

// PORT_HOST_ABI: implicit-register-arg (TextureSequence::Prepare consumes 3 params; the matched caller passes 2, the 2nd rode through on ARM).
int LakituBro::InitResources()
{
  BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov085_0213074c);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)((char*)&mModelAnim1), bmd, 1, -1);
  _ZN9Animation8LoadFileER13SharedFilePtr(data_ov085_02130744);
  _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov085_0213073c);
  _ZN11ShadowModel12InitCylinderEv((ShadowModel*)((char*)&mShadowModel1));
  _ZN11ShadowModel12InitCylinderEv((ShadowModel*)((char*)&mShadowModel2));
  _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
      *(BMD_File*)data_ov085_0213074c.f4, *(BTP_File*)data_ov085_0213073c.f4, *(BTP_File*)data_ov085_0213073c.f4);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
      (ModelAnim*)((char*)&mModelAnim1), (BCA_File*)data_ov085_02130744.f4, 0, 0x1000, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
      (TextureSequence*)((char*)&mTextureSequence), *(BTP_File*)data_ov085_0213073c.f4, 0, 0x1000, 0);
  unk_16c = 0x1000;
  unk_2d0 = unk_008 & 0xff;
  if (unk_2d0 == 0xff)
    unk_2d0 = 0;
  /* WHICH CHAIN, PER SPAWN. The opening's last unmet assertion is flags2 bit 7,
     whose only writer is func_ov085_0212d5dc -- the LAST state of the OPENING
     chain, data_ov085_02130790, which is reachable only through case 1 below and
     only while the bit is clear. An entry probe on that state printed nothing
     over 6000 frames, so the question is which arm this switch takes and with
     what param. Reported per LakituBro spawn. Inert unless SM64DS_INTRO_WATCH.
     Diagnostic only: no control flow below is changed. */
  {
    static int lb_on = -1;
    if (lb_on < 0)
        lb_on = std::getenv("SM64DS_INTRO_WATCH") ? 1 : 0;
    if (lb_on) {
        const int bit7 = (*(int*)(data_0209caa0 + 8) & 0x80) ? 1 : 0;
        std::fprintf(stderr,
                     "  [lakitu] InitResources: param unk_008 0x%x -> case %d"
                     " | flags2 bit7 %d -> chain %s\n",
                     (unsigned)unk_008, (int)unk_2d0, bit7,
                     unk_2d0 == 0 ? "021307d0 (moat)"
                     : unk_2d0 == 1 ? (bit7 ? "021307e0 (post-intro)"
                                            : "02130790 (THE OPENING)")
                     : "NONE -- no case, no chain");
    }
  }
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
