extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern int ONE_UP_MUSHROOM_MODEL_PTR[];
extern int data_ov080_021283c0[];
extern int data_ov080_021283c8[];
extern int data_ov080_0212766c[];
int func_ov080_02124458(void) {
  _ZN13SharedFilePtr7ReleaseEv(ONE_UP_MUSHROOM_MODEL_PTR);
  _ZN13SharedFilePtr7ReleaseEv(data_ov080_021283c0);
  _ZN13SharedFilePtr7ReleaseEv(data_ov080_021283c8);
  int i = 0;
  do {
    _ZN13SharedFilePtr7ReleaseEv((void*)data_ov080_0212766c[i]);
    i++;
  } while (i < 4);
  return 1;
}
