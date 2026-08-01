//cpp
#include "types.h"
// @symbol _ZN4Fish13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Fish.h"
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int data_ov100_021489cc[];
extern int* data_ov100_021473a4[];
extern int* data_ov100_021473b0[];

int Fish::InitResources()
{
  u8 v;
  _ZN9Animation8LoadFileER13SharedFilePtr(data_ov100_021489cc);
  mModelIndex = (mParam >> 4) & 7;
  v = mModelIndex;
  if (v > 2) {
    if (v < 6) unk_15c = v - 2;
    mModelIndex = 0;
  }
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov100_021473a4[mModelIndex]), 1, -1);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this)+0xd4, _ZN9Animation8LoadFileER13SharedFilePtr(data_ov100_021473b0[mModelIndex]), 0, 0x1000, 0);
  unk_159 = 1;
  unk_13c = unk_004;
  unk_14c = 0;
  unk_140 = mPosY + 0xc8000;
  unk_150 = 0;
  return 1;
}
