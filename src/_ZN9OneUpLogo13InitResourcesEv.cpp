//cpp
// @symbol _ZN9OneUpLogo13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "OneUpLogo.h"
#include "TextureSequence.h"
extern "C" {
struct SharedFilePtr { int a, file; };
extern SharedFilePtr data_ov002_02110a9c;
extern SharedFilePtr data_ov002_02110aa4;
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int c, unsigned int n);
}

int OneUpLogo::InitResources()
{
  unsigned short n;
  {
    unsigned int v = param1;
    n = (unsigned short)(v > 8 ? 7 : (v - 1));
  }
  _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_02110a9c);
  if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_02110aa4), 1, -1) == 0)
    return 0;
  TextureSequence::Prepare(*(BMD_File*)data_ov002_02110aa4.file, *(BTP_File*)data_ov002_02110a9c.file);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, (void*)data_ov002_02110a9c.file, 0x40000000, 0, n);
  mState = 0;
  mInitialPos.x = mPosX;
  mInitialPos.y = mPosY;
  mInitialPos.z = mPosZ;
  mVertSpeed = 0x14000;
  mVertAccel = -0x2000;
  mTerminalVelocity = -0x32000;
  mDelay = 0;
  mFollowActorID = 0;
  mFollowYOffset = 0;
  return 1;
}
