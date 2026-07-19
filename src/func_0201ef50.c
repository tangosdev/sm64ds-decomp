
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
struct StarEntry
{
  int m0;
  int m4;
};
extern u8 data_0209d660;
extern u8 data_0209d6c4;
extern u8 data_0209d690;
extern s16 data_0209d6d4;
extern struct StarEntry *data_0209d708;
extern int *data_0209d70c;
extern int data_0209d6fc;
extern struct StarEntry *data_0209d6f0;
extern int data_0209d6f4;
extern u8 data_0209d6bc;
extern u8 data_0209d6a8;
extern u8 data_0209d680;
extern u8 data_0209d65c;
extern u16 data_0209d6e0;
extern u8 data_0209d6b0;
void func_0201eaac(void);
int _ZN3G2S13GetBG0CharPtrEv(void);
void MultiStore_Int(int val, int *dst, int len);
void func_0201b6f8(int mode);
void func_0201b7cc(void);
void func_0201b388(int a);
void func_0201ef50(int arg)
{
  volatile int li;
  volatile int li2;
  int b;
  int new_var;
  int t;
  int idx;
  int sl;
  int cp;
  func_0201eaac();
  data_0209d660 = 0;
  data_0209d6c4 = 0;
  data_0209d690 = 1;
  cp = _ZN3G2S13GetBG0CharPtrEv();
  li = 0;
  MultiStore_Int(li, (int *) cp, 0x8000);
  if (arg < 0)
  {
    return;
  }
  data_0209d6d4 = (s16) (arg + 0x24e);
  idx = data_0209d6d4;
  data_0209d6f0 = (struct StarEntry *) (((char *) data_0209d708) + (idx * 8));
  data_0209d6f4 = ((data_0209d6fc + 0x28) + data_0209d70c[1]) + (*((int *) (((char *) data_0209d708) + (idx * 8))));
  data_0209d6a8 = 0x20;
  data_0209d680 = 0;
  data_0209d65c = 0;
  b = *((u8 *) ((((char *) data_0209d708) + (idx * 8)) + 6));
  t = 0xc - b;
  sl = (u16) (t << 10);
  data_0209d6e0 = (s16) (t << 5);
  while (data_0209d6bc != 8)
  {
    func_0201b6f8(0);
    data_0209d65c = (0x100 - ((int) data_0209d6b0)) >> 1;
    cp = _ZN3G2S13GetBG0CharPtrEv();
    new_var = cp;
    new_var = new_var + sl;
    li2 = 0;
    MultiStore_Int(li2, (int *) new_var, 0x800);
    func_0201b7cc();
    func_0201b388(0x20);
    sl = (u16) (sl + 0x800);
    data_0209d6e0 = data_0209d6e0 + 0x40;
  }

}
