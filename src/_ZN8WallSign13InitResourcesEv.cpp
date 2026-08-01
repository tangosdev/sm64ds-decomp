//cpp
// @symbol _ZN8WallSign13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WallSign.h"
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, void* file, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(char* self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, char* actor, const struct Vector3* pos, int a, unsigned int b, unsigned int c, unsigned int d);

int WallSign::InitResources()
{
  struct Vector3 v;
  void* file = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_02130858);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, file, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  v.x = data_ov085_0212f29c.x;
  v.y = data_ov085_0212f29c.y;
  v.z = data_ov085_0212f29c.z;
  _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(((char*)this) + 0x320, ((char*)this), &v, 0x78000, 0x64000, 0x4800002, 0);
  return 1;
}
