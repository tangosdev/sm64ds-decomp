
extern unsigned short *G2S_GetBG1ScrPtr(void);
void func_02005348(void *self)
{
  unsigned char *s = (unsigned char *) self;
  int i;
  unsigned int new_var;
  for (i = 0; i < 2; i++)
  {
    int v;
    unsigned short *base;
    unsigned short *row;
    int j;
    if ((i == s[0x53]) && (s[0x54] == 0))
    {
      v = 0x2000;
    }
    else
    {
      v = 0x1000;
    }
    v = (unsigned short) v;
    base = (unsigned short *) (((char *) G2S_GetBG1ScrPtr()) + 0x4c0);
    new_var = v;
    row = base + (i * 0x10);
    for (j = 0; j < 0x10; j++)
    {
      row[j] = v + (row[j] & 0x3ff);
      (&row[j])[0x20] = new_var + ((&row[j])[0x20] & 0x3ff);
      (&row[j])[0x40] = new_var + ((&row[j])[0x40] & 0x3ff);
      (&row[j])[0x60] = v + ((&row[j])[0x60] & 0x3ff);
    }

  }

}
