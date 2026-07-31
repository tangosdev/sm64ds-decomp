
typedef unsigned char u8;
typedef unsigned short u16;
extern short data_0209d6d4;
extern int data_0209d708;
extern int *data_0209d70c;
extern int data_0209d6fc;
extern int data_0209d6f0;
extern int data_0209d6f4;
extern u8 data_0209d6b0;
extern u8 data_0209d65c;
extern u8 data_0209d6a8;
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int val, int dst, int len);
extern void func_0201b6f8(int a);
extern void func_0201b100(int a);
void func_0201d6a0(int a, int b)
{
  int new_var5;
  volatile int li;
  int n;
  int new_var4;
  int p;
  int i;
  int idx = data_0209d6d4;
  int new_var;
  char *base = (char *) data_0209d708;
  u16 *ptr;
  int new_var3;
  int new_var2;
  int col;
  int cnt;
  int old;
  data_0209d6f0 = (int) (base + (idx * 8));
  data_0209d6f4 = ((data_0209d6fc + 0x28) + data_0209d70c[1]) + (*((int *) (base + (idx * 8))));
  new_var4 = 1;
  func_0201b6f8(new_var4);
  old = data_0209d6b0;
  data_0209d65c = (((b * 8) - old) & 0xf) >> new_var4;
  data_0209d6b0 = old + data_0209d65c;
  n = ((data_0209d6b0 + 7) / 8) << 6;
  new_var5 = (_ZN3G2S13GetBG0CharPtrEv() + 0x4000) + (data_0209d6a8 << 5);
  p = new_var5;
  new_var3 = b - ((old + 7) / 8);
  new_var2 = new_var3;
  li = 0;
  MultiStore_Int(li, p, n);
  col = (u16) (new_var2 >> 1);
  old = old;
  ptr = ((u16 *) (_ZN3G2S12GetBG0ScrPtrEv() + (a * 2))) + col;
  cnt = data_0209d6b0;
  cnt = (cnt + 7) / 8;
  for (i = 0; i < cnt; i++)
  {
    new_var = 0x200;
    ptr[i] = (u16) ((data_0209d6a8 + new_var) + i);
    (ptr + i)[0x20] = (u16) (((data_0209d6a8 + 0x200) + cnt) + i);
  }

  func_0201b100(0);
  data_0209d6a8 = data_0209d6a8 + ((u8) (((data_0209d6b0 + 7) / 8) << new_var4));
}
