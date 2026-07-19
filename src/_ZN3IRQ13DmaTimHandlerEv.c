typedef unsigned int u32;
typedef unsigned short u16;
extern void _ZN3IRQ11DisableIRQsEj(u32 mask);
typedef struct
{
  void (*fn)(void *);
  u32 flag;
  void *arg;
} DmaEnt;
extern DmaEnt data_020a60c4[];
extern u16 data_02099fd4[];
extern u32 data_023c0000[];
inline void (*inline_fn(DmaEnt *arg0, u32 arg1))(void *)
{
  return arg0[arg1].fn;
}

void _ZN3IRQ13DmaTimHandlerEv(u32 idx)
{
  u16 bit = data_02099fd4[idx];
  u32 mask = 1u << bit;
  void (*fn)(void *) = inline_fn(data_020a60c4, idx);
  data_020a60c4[idx].fn = 0;
  if (0 != fn)
  {
    fn(data_020a60c4[idx].arg);
  }
  *(volatile u32 *)(((int)data_023c0000 + 0x3ff8) & 0xFFFFFFFFFFFFFFFF) |= mask;
  if (data_020a60c4[idx].flag != 0)
  {
    return;
  }
  _ZN3IRQ11DisableIRQsEj(mask);
}
