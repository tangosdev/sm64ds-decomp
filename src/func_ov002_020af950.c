extern void func_ov002_020aefa4(char *self);
extern void Sound_PlayBank3(unsigned int, const void *);
extern void func_ov002_020af0c0(char *self);
extern void func_ov002_020aefb8(char *self);
extern void func_ov002_020aeee4(char *self);
extern void func_ov002_020af474(char *self);

void func_ov002_020af950(char *self)
{
  switch (*((int *)(self + 0x388)))
  {
    case 0:
      *((unsigned char *)(self + 0x38e)) = 0;
      if (*((int *)(self + 0x390)) != 0)
        return;

      *((int *)(self + 0xa8)) = 0x28000;
      *((int *)(self + 0x388)) = 3;
      *((unsigned char *)(self + 0x38e)) = 1;
      func_ov002_020aefa4(self);
      Sound_PlayBank3(0x68, self + 0x74);
      *((unsigned int *)(((long long)(int)(self + 0xb0)) &
                         0xFFFFFFFFFFFFFFFFLL)) &= ~1;
      return;

    case 1:
      func_ov002_020af0c0(self);
      func_ov002_020aefb8(self);
      return;

    case 3:
      func_ov002_020aefb8(self);
      if (*((unsigned short *)(self + 0x100)) > 0x11)
        func_ov002_020aeee4(self);
      func_ov002_020af474(self);
      if (*((unsigned short *)(self + 0x100)) != 0x25)
        return;

      *((unsigned int *)(((long long)(int)(self + 0x128)) &
                         0xFFFFFFFFFFFFFFFFLL)) &= ~1;
      *((int *)(self + 0x388)) = 1;
      *((int *)(self + 0x9c)) = 0;
      *((int *)(self + 0x98)) = 0xa000;
      return;
  }
}
