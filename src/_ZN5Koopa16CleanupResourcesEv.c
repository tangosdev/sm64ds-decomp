
struct SharedFilePtr
{
  unsigned int data[4];
};
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *);
extern void UnloadBlueCoinModel(void *c);
extern struct SharedFilePtr *data_ov062_0211cee0[];
extern struct SharedFilePtr *data_ov062_0211ced8[];
extern struct SharedFilePtr *data_ov062_0211cee8[];
int _ZN5Koopa16CleanupResourcesEv(char *c)
{
  int b = (int) ((*((unsigned short *) (c + 0xc))) == 0xcc);
  if (b == 0)
  {
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211cee0[*((int *) (c + 0x394))]);
  }
  _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211ced8[*((int *) (c + 0x394))]);
  {
    int i = 0;
    do
    {
      _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211cee8[(long) i]);
      i++;
    }
    while (i < 9);
  }
  UnloadBlueCoinModel(c);
  return 1;
}
