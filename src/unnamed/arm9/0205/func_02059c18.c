#include "types.h"
extern long long func_02059650(void *obj);
extern void func_02056e4c(u32 idx, u32 handler, u32 arg);
extern void func_02059824(void);
extern void _ZN3IRQ10EnableIRQsEj(unsigned int mask);
#ifdef _MSC_VER
/* The ROM's 4-byte layout for the 64-bit field(s) of the alarm record (see
 * src/func_02059a60.c); mwccarm never sees this arm. */
#pragma pack(push, 4)
#endif
typedef struct Obj
{
  char pad0c[0xc];
  long long t;
} Obj;
#ifdef _MSC_VER
#pragma pack(pop)
#endif
void func_02059c18(Obj *obj)
{
  long long v;
  long long sub;
  u16 timer;
  sub = func_02059650(obj);
  *((volatile u16 *) 0x4000106) = 0;
  v = obj->t - sub;
  func_02056e4c(1, (u32) func_02059824, 0);
  if (v < 0)
  {
    timer = 0xfffe;
  }
  else
    if (v < 0x10000)
  {
    timer = (u16) (~((u32) v));
  }
  else
  {
    timer = 0;
  }
  *((volatile u16 *) 0x4000104) = timer;
  *((volatile u16 *) 0x4000106) = 0xc1;
  _ZN3IRQ10EnableIRQsEj(0x10);
}
