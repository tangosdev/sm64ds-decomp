
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef long long s64;
struct Vector3
{
  int x;
  int y;
  int z;
};
extern short data_02082214[];
extern void *data_ov030_02115d08[];
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
extern int _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(void *self, struct Vector3 *a, struct Vector3 *out, int flag);
int func_ov030_02112c14(void *arg0)
{
  u8 *new_var;
  u8 *c = (u8 *) arg0;
  int *pos;
  int *py;
  int *pz;
  struct Vector3 v;
  u8 *other;
  u16 ang;
  s16 s;
  int mul = 0x4b000;
  int rnd = 0x800;
  *((int *) ((int) (((s64) ((int) (c + 0xb0))) & 0xFFFFFFFFFFFFFFFFLL))) &= ~0x80000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
  *((int *) (c + 0x98)) = 0xa000;
  *((int *) (c + 0xa8)) = 0;
  other = *((u8 **) (c + 0x3a8));
  pos = (int *) ((int) (((s64) ((int) (c + 0x5c))) & 0xFFFFFFFFFFFFFFFFLL));
  s = *((s16 *) (other + 0x8e));
  *((s16 *) (c + 0x8e)) = s;
  s = *((s16 *) (c + 0x8e));
  py = (int *) ((int) (((s64) ((int) (c + 0x60))) & 0xFFFFFFFFFFFFFFFFLL));
  pz = (int *) ((int) (((s64) ((int) (c + 0x64))) & 0xFFFFFFFFFFFFFFFFLL));
  *((s16 *) (c + 0x94)) = s;
  other = *((u8 **) (c + 0x3a8));
  {
    int *op = (int *) ((int) (((s64) ((int) (other + 0x5c))) & 0xFFFFFFFFFFFFFFFFLL));
    int t0 = op[0];
    *((int *) (c + 0x5c)) = t0;
    int t1 = op[1];
    *((int *) (c + 0x60)) = t1;
    int t2 = op[2];
    *((int *) (c + 0x64)) = t2;
  }
  ang = *((u16 *) (c + 0x8e));
  s = data_02082214[(ang >> 4) * 2];
  *pos = (*pos) + ((int) ((((((s64) s) * mul) + rnd) >> 1) >> 11));
  *py = (*py) + 0x50000;
  ang = *((u16 *) (c + 0x8e));
  s = data_02082214[((ang >> 4) * 2) + 1];
  *pz = (*pz) + ((int) (((((s64) s) * mul) + rnd) >> 12));
  other = *((u8 **) (c + 0x3a8));
  {
    int oy = *((int *) (other + 0x60));
    int oz = *((int *) (new_var = other + 0x64));
    mul = oy + 0x50000;
    v.x = *((int *) (other + 0x5c));
    v.y = mul;
    v.z = oz;
  }
  _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(c, &v, (struct Vector3 *) pos, 1);
  *((int *) (c + 0xd0)) = 0;
  *((int *) (c + 0x3a8)) = 0;
  *((int *) (c + 0x3b4)) = 7;
  return 1;
}
