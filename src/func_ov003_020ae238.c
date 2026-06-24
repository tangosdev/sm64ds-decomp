
int SublevelToLevel(int i);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int b, void *attr, int x, int y, int a, int cc, int fx, int t, int e, int f);
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int b, void *attr, int x, int y, int a, int cc, void *m);
void func_ov003_020ae1a4(void *sl, int r);
extern signed char data_02092110[];
extern signed char data_0209f2f4[];
extern void *func_020ab948[];
extern void *func_020ab9c8;
extern void *func_020aba70[];
void func_ov003_020ae238(char *sl)
{
  int sb;
  int r8;
  if (SublevelToLevel(data_02092110[0]) >= 0xf)
  {
    r8 = 0xa0;
    sb = 0x50;
  }
  else
  {
    sb = 0x10;
    r8 = 0xac;
  }
  _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, func_020ab948[*((unsigned char *) (sl + 0x116))], sb, r8 + 8, -1, -1, 0x1000, 0x1000, 0, -1);
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &func_020ab9c8, sb + 0x10, r8 + 8, -1, -1, 0);
  func_ov003_020ae1a4(sl, (unsigned short) data_0209f2f4[0]);
  {
    int i = 0;
    sb += 0x18;
    do
    {
      signed char d = *((signed char *) ((sl + i) + 0x121));
      if (d >= 0)
      {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, func_020aba70[d], sb, r8, 8, -1, 0);
        sb += 9;
      }
      i++;
    }
    while (i < 3);
  }
}
