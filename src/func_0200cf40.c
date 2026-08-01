#include "types.h"
extern void func_0200cb58(void *obj, int index);
extern void func_0200d954(void *c, int arg);
extern void func_0200cce4(void *c);
extern struct E *GetViewObj(int idx);
extern int data_0208715c;
struct E
{
  s16 f0;
  s16 f2;
  s16 f4;
  s16 f6;
  s16 f8;
  s16 fa;
};
void func_0200cf40(char *c)
{
  int new_var;
  *((u8 *) (c + 0x10c)) = 0;
  if (c)
  {
  }
  *((u8 *) (c + 0x10d)) = 0;
  *((u8 *) (c + 0x10e)) = 0xff;
  *((u8 *) (c + 0x10f)) = 0xbf;
  *((int *) (c + 0xf8)) = 0x1555;
  *((int *) (c + 0xfc)) = 0x1000;
  *((int *) (c + 0x100)) = 0x1388000;
  *((int *) (c + 0x140)) = (int) (&data_0208715c);
  *((int *) (c + 0x144)) = 0;
  if ((*((int *) (c + 8))) != 0xf)
  {
    struct E *v;
    int x;
    int y;
    int z;
    func_0200cb58(c, 0xa);
    *((s16 *) (c + 0x17a)) = *((s16 *) ((*((char **) (c + 0x13c))) + 0x24));
    func_0200d954(c, *((s16 *) (c + 0x17a)));
    func_0200cce4(c);
    v = GetViewObj((*((int *) (c + 8))) & 0xff);
    x = v->f2;
    z = v->f6;
    new_var = z << 12;
    y = v->f4;
    *((int *) (c + 0x8c)) = x << 12;
    *((int *) (c + 0x90)) = y << 12;
    *((int *) (c + 0x94)) = new_var;
    *((s16 *) (c + 0x182)) = v->fa + 0x8000;
    return;
  }
  func_0200cb58(c, 9);
  *((s16 *) (c + 0x17a)) = *((s16 *) ((*((char **) (c + 0x13c))) + 0x24));
  func_0200d954(c, *((s16 *) (c + 0x17a)));
  func_0200cce4(c);
}
