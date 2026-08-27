
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(int self);
extern void _Z14ApproachLinearRiii(int *dst, int a, int b);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int fix, int d, int e, int vec, int cb);
extern int _ZN7fBase_c18MarkForDestructionEv(int self);
int func_ov064_02118644(unsigned int c)
{
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144) != 0)
  {
    if ((*((int *) (c + 0x98))) == 0)
    {
      *((int *) (c + 0x98)) = 0x16000;
      *((int *) (c + 0xa8)) = 0x32000;
    }
    else
    {
      _Z14ApproachLinearRiii((int *) (c + 0x98), 0x8000, 0x3000);
      *((int *) (c + 0xa8)) = ((*((int *) (c + 0xa8))) * (-7)) / 10;
    }
  }
  *((int *) (c + 0x314)) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*((int *) (c + 0x314)), 0x4d, *((int *) (c + 0x5c)), (*((int *) (c + 0x60))) + 0x37000, *((int *) (c + 0x64)), 0, 0);
  *((int *) (c + 0x318)) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*((int *) (c + 0x318)), 0x4e, *((int *) (c + 0x5c)), (*((int *) (c + 0x60))) + 0x37000, *((int *) (c + 0x64)), 0, 0);
  if ((*((unsigned short *) (c + 0x100))) == 0)
  {
    _ZN7fBase_c18MarkForDestructionEv(c);
  }
  return 1;
}
