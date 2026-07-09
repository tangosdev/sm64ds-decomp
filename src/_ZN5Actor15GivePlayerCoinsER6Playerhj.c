
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;
struct Vector3
{
  s32 x;
  s32 y;
  s32 z;
};
struct Vector3_16
{
  s16 x;
  s16 y;
  s16 z;
};
extern u32 data_02075238[];
extern u16 data_02075230[];
extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, struct Vector3 *v);
extern void GiveCoins(s32 a, s32 b);
extern void _ZN6Player4HealEi(void *p, s32 amt);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 param, struct Vector3 *v, struct Vector3_16 *vr, s32 a, s32 b);
void _ZN5Actor15GivePlayerCoinsER6Playerhj(void *c, void *p, unsigned char h, u32 j)
{
  s32 amt;
  void *spawned;
  struct Vector3 vec;
  struct Vector3 *src;
  if (p == 0)
  {
    return;
  }
  if (h == 0)
  {
    return;
  }
  if (j >= 3)
  {
    return;
  }
  if ((*((unsigned char *) (((char *) p) + 0x706))) != 0)
  {
    _ZN5Sound9PlayBank3EjRK7Vector3(0x12, (struct Vector3 *) (((char *) c) + 0x74));
  }
  else
  {
    _ZN5Sound9PlayBank3EjRK7Vector3(0x11, (struct Vector3 *) (((char *) c) + 0x74));
  }
  amt = h * data_02075238[j];
  GiveCoins(*((unsigned char *) (((char *) p) + 0x6d8)), amt);
  _ZN6Player4HealEi(p, amt << 8);
  src = (struct Vector3 *) (((int) p + 0x5c) & 0xFFFFFFFFFFFFFFFF);
  vec.x = src->x;
  vec.y = src->y;
  vec.z = src->z;
  vec.y = vec.y + 0x96000;
  spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(data_02075230[j], 8, &vec, (struct Vector3_16 *) 0, *((signed char *) (((char *) p) + 0xcc)), -1);
  if (spawned != 0)
  {
    *((void **) (((char *) spawned) + 0x39c)) = p;
  }
}
