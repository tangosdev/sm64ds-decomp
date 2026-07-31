#include "types.h"
extern int __aeabi_idiv(int a, int b);
extern void func_ov080_02126124(u32 *c);
void func_ov080_02126a54(char *sl)
{
  int sb;
  int fp;
  int j;
  int nb;
  int i;
  sb = 0;
  fp = 0;
  j = 0;
  if (((int) (*((u8 *) (sl + 0x1bb)))) > 0)
  {
    do
    {
      i = 0;
      if (((int) (*((u8 *) (sl + 0x1ba)))) > 0)
      {
        do
        {
          char *r6 = (*((char **) (sl + 0x1a0))) + (((j * ((int) (*((u8 *) (sl + 0x1bb))))) + i) * 0x18);
          *((int *) (r6 + 0)) = sb;
          *((int *) (r6 + 4)) = (unsigned long) fp;
          *((int *) (r6 + 8)) = 0;
          *((int *) (r6 + 0x10)) = 0x1ff00000;
          int qa = __aeabi_idiv(0x80000, ((int) (*((u8 *) (sl + 0x1ba)))) - 1);
          int qb = __aeabi_idiv(0x80000, ((int) (*((u8 *) (sl + 0x1bb)))) - 1);
          int va = qa * i;
          int vb = 0x80000 - (qb * j);
          *((int *) (r6 + 0x14)) = ((u16) ((s16) (va >> 8))) | ((((u16) ((s16) (vb >> 8))) << 1) << 15);
          nb = (int) (*((u8 *) (sl + 0x1ba)));
          if (i == (nb - 2))
          {
            sb = (((u8) ((*((int *) (sl + 8))) & 0xf)) * 0x64000) + 0x64000;
          }
          else
          {
            sb += __aeabi_idiv((((u8) ((*((int *) (sl + 8))) & 0xf)) * 0x64000) + 0x64000, nb - 1);
          }
          i++;
        }
        while (i < nb);
      }
      nb = (int) (*((u8 *) (sl + 0x1bb)));
      sb = 0;
      if (j == (nb - 2))
      {
        fp = (((u8) ((((*((int *) (sl + 8))) & 0xFFFFFFFF) >> 4) & 0xf)) * 0x64000) + 0x64000;
      }
      else
      {
        fp += __aeabi_idiv((((u8) ((((*((int *) (sl + 8))) & 0xFFFFFFFFu) >> 4) & 0xf)) * 0x64000) + 0x64000, nb - 1);
      }
      j++;
    }
    while (j < nb);
  }
  func_ov080_02126124((u32 *) sl);
}
