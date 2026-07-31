//cpp
// @symbol _ZN6Player16IncMegaKillCountEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct Vec3
{
  int x;
  int y;
  int z;
};
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void *v);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int id, unsigned int p, struct Vec3 *pos, void *rot, int a, int b);

void Player::IncMegaKillCount()
{
  struct Vec3 v;
  char *new_var2;
  int new_var3;
  char *a;
  char *st;
  int y;
  char *new_var;
  if ((*((unsigned char *) ((char *)&mIsMega))) == 0)
  {
    return;
  }
  ++(*((unsigned short *) (((int) ((char *)&unk_6d0)))));
  new_var3 = -1;
  st = ((char *)this);
  st = st + 0x600;
  if ((*((unsigned short *) (st + 0xd0))) >= 8)
  {
    *((unsigned short *) (st + 0xd0)) = 8;
    _ZN5Sound9PlayBank3EjRK7Vector3(0x6e, ((char *)this) + 0x74);
    GiveLives(1);
  }
  v.x = *((int *) ((char *)&mPosX));
  y = *((int *) ((char *)&mPosY));
  v.y = y;
  new_var = ((char *)this);
  v.z = *((int *) (new_var + 0x64));
  v.y = y + 0x190000;
  new_var2 = (char *) _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x14b, *((unsigned short *) (((char *)&unk_600) + 0xd0)), &v, 0, *((signed char *) ((char *)&mAreaId)), new_var3);
  a = new_var2;
  if (a == 0)
  {
    return;
  }
  *((unsigned short *) ((a + 0x100) + 0x4c)) = 0;
  func_ov002_020f0e54(a, 0);
}
