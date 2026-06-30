int SublevelToLevel(int i);
unsigned char NumStars(void);
void func_ov003_020ae1a4(void *sl, int r);
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int b, void *attr, int x, int y, int a, int cc, void *m);
extern signed char data_02092110[];
extern void *func_020aba70[];
extern void *func_020ab9c8;
extern void *func_020abad0;
void func_ov003_020ae0b0(char *sl)
{
  int sb;
  int r8;
  if (SublevelToLevel(data_02092110[0]) >= 0xf)
  {
    r8 = 0xa0;
    sb = 0xb8;
  }
  else
  {
    r8 = 0xac;
    sb = 0xf4;
  }
  func_ov003_020ae1a4(sl, NumStars());
  {
    int i = 2;
    do
    {
      signed char d = *((signed char *) ((sl + i) + 0x121));
      if (d >= 0)
      {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, func_020aba70[d], sb, r8, 8, -1, 0);
        sb -= 9;
      }
      i--;
    }
    while (i >= 0);
  }
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &func_020ab9c8, sb, r8 + 8, -1, -1, 0);
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &func_020abad0, sb - 0x10, r8 + 8, -1, -1, 0);
}
