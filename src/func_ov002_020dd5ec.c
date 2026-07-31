#include "types.h"
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *c, u32 anim, int a, Fix12 b, u32 d);
extern int _ZNK6Player14GetBodyModelIDEjb(void *c, u32 id, int b);
extern int RandomIntInternal(int *seed);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void *v);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void *v);
extern u16 data_0209f49c[];
extern u8 data_020a0e40;
extern int data_ov002_0210a5c0[];
extern u16 data_ov002_02109dbc[];
extern int data_ov002_0210e160;
extern int data_ov002_020ff194[];
void func_ov002_020dd5ec(char *c)
{
  int model;
  int *new_var3;
  int *p;
  int r;
  int idx;
  int *new_var;
  int new_var2;
  if ((*((u16 *) (((char *) data_0209f49c) + (data_020a0e40 * 0x18)))) & 2)
  {
    *((u8 *) (c + 0x6e2)) = 2;
  }
  _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_0210a5c0[*((u8 *) (c + 0x6e2))], 0x40000000, 0x1000, 0);
  model = _ZNK6Player14GetBodyModelIDEjb(c, (*((int *) (c + 8))) & 0xff, 0);
  new_var2 = -0x4000;
  new_var = (int *) ((c + (model * 4)) + 0xdc);
  new_var3 = (int *) ((*new_var) + 0x50);
  if ((c && c) && c)
  {
  }
  p = new_var3;
  p[2] = 0;
  *((u16 *) (c + 0x6aa)) = data_ov002_02109dbc[*((u8 *) (c + 0x6e2))];
  if ((*((u8 *) (c + 0x6e2))) == 2)
  {
    if ((((*((u16 *) (((char *) data_0209f49c) + (data_020a0e40 * 0x18)))) & 2) != 0) || ((*((u8 *) (c + 0x6de))) != 0))
    {
      *((int *) (c + 0xa8)) = 0x20000;
      *((int *) (c + 0x9c)) = new_var2;
    }
    else
    {
      *((int *) (c + 0xa8)) = 0xc000;
      *((int *) (c + 0x9c)) = -0x1800;
    }
    *((u8 *) (c + 0x6de)) = 1;
    *((u8 *) (c + 0x6df)) = 0;
  }
  idx = *((u8 *) (c + 0x6e2));
  if (idx != 2)
  {
    r = RandomIntInternal(&data_ov002_0210e160);
    idx = (idx + (((unsigned int) r) >> 4)) & 1;
  }
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*((u8 *) (c + 0x6d9)), data_ov002_020ff194[idx], (void *) (c + 0x74));
  if ((*((u8 *) (c + 0x703))) != 0)
  {
    _ZN5Sound9PlayBank0EjRK7Vector3(0xd5, (void *) (c + 0x74));
    return;
  }
  if ((*((u8 *) (c + 0x6e2))) != 2)
  {
    _ZN5Sound9PlayBank0EjRK7Vector3(7, (void *) (c + 0x74));
  }
  else
  {
    _ZN5Sound9PlayBank0EjRK7Vector3(8, (void *) (c + 0x74));
  }
}
