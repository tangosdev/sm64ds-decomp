extern void func_02012694(int a, void *b);
extern void _ZN10dBgCh_Actr15ClearGroundFlagEv(void *);
extern int data_ov084_02130248[];
void func_ov084_02129c9c(char *c)
{
  char *new_var;
  func_02012694(0x118, c + 0x74);
  *((int *) (c + 0x434)) = 2;
  *((int *) (c + 0x98)) = 0;
  new_var = c;
  *((int *) (new_var + 0xa8)) = data_ov084_02130248[*((int *) (new_var + 0x460))];
  _ZN10dBgCh_Actr15ClearGroundFlagEv(new_var + 0x1b4);
  *((int *) ((char *)(((int)(new_var + 0x198)) + 0))) |= 4;
}
