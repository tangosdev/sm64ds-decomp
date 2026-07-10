
extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern int _ZN16MeshColliderBase7DisableEv(void *);
extern int _ZN13SharedFilePtr7ReleaseEv(void *);
extern unsigned long UnloadSilverStarAndNumber(void);
struct SE
{
  void *p;
  char pad[8];
};
extern struct SE data_ov002_021098e8[];
extern struct SE data_ov002_021098ec[];
extern char data_ov002_0211092c[];
int _ZN10StarSwitch16CleanupResourcesEv(void *c)
{
  int b;
  unsigned char *new_var2;
  struct SE *new_var;
  if (_ZN16MeshColliderBase9IsEnabledEv(((char *) c) + 0x124))
  {
    _ZN16MeshColliderBase7DisableEv(((char *) c) + 0x124);
  }
  new_var = &data_ov002_021098e8[*(new_var2 = (unsigned char *) (((char *) c) + 0x34c))];
  _ZN13SharedFilePtr7ReleaseEv((*new_var).p);
  _ZN13SharedFilePtr7ReleaseEv(data_ov002_021098ec[*new_var2].p);
  b = *((unsigned short *) (((char *) c) + 0xc));
  b = b == 0xc;
  if (b)
  {
    UnloadSilverStarAndNumber();
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0211092c);
  }
  return 1;
}
