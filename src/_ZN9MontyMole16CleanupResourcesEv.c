extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern int data_ov002_0210d9d8[];
extern int data_ov080_021283c0[];
extern int data_ov080_021283c8[];
extern int data_ov080_0212766c[];
int _ZN9MontyMole16CleanupResourcesEv(void) {
  _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d8);
  _ZN13SharedFilePtr7ReleaseEv(data_ov080_021283c0);
  _ZN13SharedFilePtr7ReleaseEv(data_ov080_021283c8);
  int i = 0;
  do {
    _ZN13SharedFilePtr7ReleaseEv((void*)data_ov080_0212766c[i]);
    i++;
  } while (i < 4);
  return 1;
}
