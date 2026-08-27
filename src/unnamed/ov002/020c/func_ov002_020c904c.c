
typedef unsigned int u32;
typedef int s32;
typedef unsigned char u8;
struct Vector3
{
  s32 x;
  s32 y;
  s32 z;
};
extern void _Z14ApproachLinearRiii(int *dst, int target, int step);
extern int Math_Function_0203b14c(char *p, int a, int b, int c, int d);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char *self, u32 anim, int a, int fix, u32 b);
extern int _ZN6Player6IsAnimEj(char *self, u32 anim);
extern int _ZNK6Player14GetBodyModelIDEjb(char *self, u32 a, int b);
extern int _ZNK9Animation12WillHitFrameEi(char *self, int frame);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const struct Vector3 *pos);
int func_ov002_020c904c(char *c)
{
  int mid;
  char *anim;
  _Z14ApproachLinearRiii((int *) (c + 0x5c), *((int *) (c + 0x744)), 0x8000);
  _Z14ApproachLinearRiii((int *) (c + 0x64), *((int *) (c + 0x74c)), 0x8000);
  if (Math_Function_0203b14c(c + 0x60, (*((int *) (c + 0x644))) + 0x190000, 0x200, 0x64000, 0x10000) == 0)
  {
    *((u8 *) (c + 0x6e3)) = 0xc;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x56, 0, 0x1000, 0);
    *((u8 *) (c + 0x70c)) = 0;
  }
  if (_ZN6Player6IsAnimEj(c, 0x56) != 0)
  {
    mid = _ZNK6Player14GetBodyModelIDEjb(c, (*((int *) (c + 8))) & 0xff, 0);
    anim = (*((char **) ((c + (mid * 4)) + 0xdc))) + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 0) != 0)
    {
      _ZN5Sound9PlayBank0EjRK7Vector3(6, (struct Vector3 *) (c + 0x74));
    }
  }
  return 0;
}
