typedef unsigned short u16;
typedef unsigned int u32;
extern long long func_02059650(void *obj);
extern void func_02056e4c(u32 idx, u32 handler, u32 arg);
extern void func_02059824(void);
extern void _ZN3IRQ10EnableIRQsEj(unsigned int mask);
typedef struct Obj
{
  char pad0c[0xc];
  long long t;
} Obj;
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
