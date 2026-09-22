
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c);
extern void ApproachAngle(short *a, short b, short c, short d, int e);
extern short data_02082214[];
void func_ov074_021216f4(char *c)
{
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x40c))
  {
    *((short *) (((int) c + 0x5f4))) += *((short *) (c + 0x5f6));
    *((int *) (c + 0x5c)) = (int) (((((long long) data_02082214[((*((unsigned short *) (c + 0x5f4))) >> 4) << 1]) * 0x546000) + 0x800) >> 12);
    *((int *) (c + 0x64)) = (int) (((((long long) data_02082214[(((*((unsigned short *) (c + 0x5f4))) >> 4) << 1) + 1]) * 0x546000) + 0x800) >> 12);
  }
  ApproachAngle((short *) (c + 0x8e), (short) (((*((int *) (c + 0x5f0))) * 0x4500) + (*((short *) (c + 0x5f4)))), 0xa, 0x800, 0x10);
}
