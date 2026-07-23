
#pragma opt_common_subs off
extern void MultiCopyHalf(void *src, void *dst, unsigned int count);
extern int data_ov002_021002a0[];
void func_ov002_020f39ec(char *self, int idx)
{
  char pad[4];
  (void)pad[0];
  int off = idx * 0x4c;
  unsigned short *p32 = (unsigned short *)(self + 0x32);
  unsigned short *ctr = (unsigned short *)((char *)p32 + off);
  *ctr = (unsigned short)(*ctr + 1);
  if (*ctr < 4) return;
  int zero = 0;
  *ctr = (unsigned short)zero;
  {
    unsigned short *p3a = (unsigned short *)(self + 0x3a);
    unsigned short *ctr2 = (unsigned short *)((char *)p3a + off);
    *ctr2 = (unsigned short)(*ctr2 + 1);
    char *row = self + off;
    if (*((unsigned char *)(row + 0x4a)) != 0) {
      if (*ctr2 >= 0x10) *ctr2 = 0x10;
    } else if (*ctr2 >= 0x20) {
      *ctr2 = (unsigned short)zero;
    }
  }

  int i = 0;
  int acc = i;
  int dst_off = i;
  char *row = self + off;
  unsigned base = 0x6600000u;
  base += 0x2000u;
  base += (unsigned)(idx << 8);
  int n100 = 0x100;
  for (; i < 4; i++) {
    int v = data_ov002_021002a0[*((unsigned short *)(row + 0x3a))];
    int src_off = acc + ((v & 7) << 8);
    if (v & 8) src_off += 0x2000;
    MultiCopyHalf((void *)((*((int *)(self + 0x510))) + src_off), (void *)(base + (unsigned)dst_off), (unsigned)n100);
    acc += 0x800;
    dst_off += 0x400;
  }
}
