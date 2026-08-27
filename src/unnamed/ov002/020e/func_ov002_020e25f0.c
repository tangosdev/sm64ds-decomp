
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int bank, void *pos);
extern int RandomIntInternal(int *seed);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, void *pos);
extern int data_ov002_0210e160[];
extern unsigned int data_ov002_020ff284[];
inline int inline_fn(int *arg0)
{
  return RandomIntInternal(arg0);
}

void func_ov002_020e25f0(char *c, int a)
{
  if ((*((unsigned char *) (c + 0x6f9))) == 0)
  {
    _ZN5Sound9PlayBank0EjRK7Vector3((*((int *) (c + 0x66c))) + 0x30, c + 0x74);
  }
  else
  {
    _ZN5Sound9PlayBank0EjRK7Vector3(0xa0, c + 0x74);
  }
  int idx = (a * 2) + (inline_fn(data_ov002_0210e160) & 1);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*((unsigned char *) (c + 0x6d9)), data_ov002_020ff284[idx], c + 0x74);
}
