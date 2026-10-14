#include "types.h"
extern int func_0206da18(void);
extern int func_0206d9cc(void);
extern void *func_02057158(unsigned int size);
extern int func_020570b8(u16 *p);
extern void _ZN3IRQ11SetFIQStateEb(int x); /* 0x0206d9dc, inside func_0206d9cc */
extern void _ZN3IRQ11SetIRQStateEb(int x); /* 0x0206da28, inside func_0206da18 */
int func_0206cee0(int *out)
{
  int v8 = func_0206da18();
  int v7 = func_0206d9cc();
  int new_var;
  int v6 = 0;
  int new_var2;
  int v5 = 0;
  if (func_02057158(0x70) == 0)
  {
    v5 = 1;
    new_var = 1;
    v6 = new_var;
  }
  else
    if (func_020570b8((u16 *) 0x27fffe8) & 0x40)
  {
    v6 = 1;
  }
  new_var2 = v8;
  if (0 != v6)
  {
    int f = 0;
    if (0, new_var2)
    {
      f |= 1;
    }
    if (v7)
    {
      f |= 2;
    }
    if (v5)
    {
      f |= 0x20;
    }
    *out = f;
  }
  else
  {
    _ZN3IRQ11SetFIQStateEb(v7);
    _ZN3IRQ11SetIRQStateEb(v8);
    *out = 0;
  }
  return v6;
}
