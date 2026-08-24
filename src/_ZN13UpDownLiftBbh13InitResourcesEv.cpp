//cpp
#include "types.h"
// @symbol _ZN13UpDownLiftBbh13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "UpDownLiftBbh.h"
/* _ZN13UpDownLiftBbh13InitResourcesEv at 0x021365d8 (ov095), size 0x18c
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 * natural signed /2 spelling; mwcc synthesizes add/lsr#31/asr#1 itself and
 * self-schedules the byte store into the ROM slot */
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(int sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *c);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(int sfp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *mc, void *kcl, void *mtx, int fix, short s, int clps);
extern void func_020393d4(void *p, void *v);
extern void func_020393c4(void *p, void *v);
}
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern int data_ov095_02136f68[];
extern int data_ov095_02136f74[];

int UpDownLiftBbh::InitResources()
{
  int idx;
  int *new_var;
  u16 t;
  int b;
  *((u8 *) ((char *)&unk_349)) = 0;
  t = *((u16 *) ((char *)&unk_00c));
  b = (int) (t == 0x20);
  if (b != 0)
  {
    *((int *) ((char *)&mVariant)) = 0;
    goto load;
  }
  b = (int) (t == 0x21);
  if (b != 0)
  {
    *((int *) ((char *)&mVariant)) = 1;
    goto load;
  }
  b = (int) (t == 0x83);
  if (b != 0)
  {
    *((int *) ((char *)&mVariant)) = 2;
    *((u8 *) ((char *)&unk_349)) = 1;
    goto load;
  }
  return 0;
  load:
  idx = *((int *) ((char *)&mVariant));

  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov095_02136f68[idx]), 1, -1);
  new_var = &_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));
  idx = *((int *) ((char *)&mVariant));
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char *)this) + 0x124, _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov095_02136f74[idx]), ((char *)this) + 0x2ec, 0x199, *((s16 *) ((char *)&unk_08e)), data_ov095_021375a4[idx]);
  func_020393d4(((char *)this) + 0x124, new_var);
  func_020393c4(((char *)this) + 0x124, &func_ov095_02136788);
  *((int *) ((char *)&mRider)) = 0;
  *((int *) ((char *)&mState)) = 0;
  if ((*((u8 *) ((char *)&unk_349))) == 2)
  {
    *((int *) ((char *)&mTopY)) = (*((int *) ((char *)&unk_060))) + ((*((u16 *) ((char *)&unk_096))) << 12);
  }
  else
  {
    *((int *) ((char *)&mTopY)) = *((int *) ((char *)&unk_060));
  }
  *((int *) ((char *)&mBottomY)) = (*((int *) ((char *)&mTopY))) - ((*((u16 *) ((char *)&unk_092))) << 12);
  *((int *) ((char *)&mMiddleY)) = ((*((int *) ((char *)&mTopY))) + (*((int *) ((char *)&mBottomY)))) / 2;
  *((u8 *) ((char *)&unk_346)) = 0;
  *((u8 *) ((char *)&mIsArmed)) = 1;
  *((u8 *) ((char *)&mIsRidden)) = 0;
  *((int *) ((char *)&mSoundHandle)) = 0;
  return 1;
}
