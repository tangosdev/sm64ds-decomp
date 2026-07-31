//cpp
// @symbol _ZN6Goomba16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Goomba.h"
extern void UnloadBlueCoinModel(void* p);
extern void _ZN13SharedFilePtr7ReleaseEv(void* p);
extern void _ZN8CapEnemy14UnloadCapModelEv(char* c);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern char data_ov084_02130cf8[];
extern void* data_ov084_02130278[];

int Goomba::CleanupResources()
{
  int i;
  if (mGoombaType == 2)
    UnloadBlueCoinModel(((char*)this));
  _ZN13SharedFilePtr7ReleaseEv(data_ov084_02130cf8);
  for (i = 0; i < 7; ++i)
    _ZN13SharedFilePtr7ReleaseEv(data_ov084_02130278[i]);
  if ((unsigned char)(unk_464 + 0xff) <= 1)
    UnloadSilverStarAndNumber();
  _ZN8CapEnemy14UnloadCapModelEv(((char*)this));
  if (mGoombaType == 3) {
    unsigned int id = unk_43c;
    if (id != 0) {
      char* a = _ZN5Actor10FindWithIDEj(id);
      if (a != 0) {
        unsigned char *p = (unsigned char*)(((int)a + 0x602) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p - 1;
      }
    }
  }
  return 1;
}
