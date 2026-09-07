
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(char *c);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(char *c);
extern void func_ov002_020baf80(char *c);
extern void func_ov002_020bbd5c(char *c, int i);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(char *thiz, char *actor, int b, int d, unsigned int e, unsigned int f);
extern int _ZN4dBgW9IsEnabledEv(char *c);
extern void _ZN4dBgW7DisableEv(char *c);
extern void _ZN5dCc_c5ClearEv(char *c);
void func_ov002_020bae9c(char *c)
{
  *((unsigned char *) (c + 0x590)) = 1;
  *((unsigned char *) (c + 0x591)) = 0x1e;
  *((unsigned char *) (c + 0x58c)) = 0;
  *((int *) (c + 0x584)) = 0;
  *((int *) (c + 0x588)) = 0;
  *((int *) (c + 0x5c)) = *((int *) (c + 0x3b0));
  *((int *) (c + 0x60)) = *((int *) (c + 0x3b4));
  *((int *) (c + 0x64)) = *((int *) (c + 0x3b8));
  *((short *) (c + 0x8c)) = *((short *) (c + 0x3bc));
  *((short *) (c + 0x8e)) = *((short *) (c + 0x3be));
  *((short *) (c + 0x90)) = *((short *) (c + 0x3c0));
  *((unsigned char *) (c + 0x58e)) = 2;
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  func_ov002_020baf80(c);
  func_ov002_020bbd5c(c, 0);
  *((unsigned char *) (c + 0x31c)) = 0;
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x320, c, 0x64000, 0x64000, 0x4800002, 0x41000);
  if (_ZN4dBgW9IsEnabledEv(c + 0x124))
  {
    _ZN4dBgW7DisableEv(c + 0x124);
  }
  _ZN5dCc_c5ClearEv(c + 0x320);
}
