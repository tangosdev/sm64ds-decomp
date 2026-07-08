extern void ShowArea(int idx);
extern int func_02012694(int a, void *b);
extern char *CAMERA;
inline short inline_fn(char *arg0, int arg1)
{
  return *((short *) (arg0 + arg1));
}

int func_ov100_02144950(char *c)
{
  *((int *) (c + 0x12c)) = 0;
  ShowArea((signed char) inline_fn(c, 0x8c));
  ShowArea((signed char) inline_fn(c, 0x90));
  *(int *)(((int)CAMERA + 0x154) & 0xFFFFFFFFFFFFFFFFull) |= 0xc00;
  func_02012694(((*((int *) (c + 8))) == 0x10) ? (6) : (4), c + 0x74);
  *((unsigned char *) (c + 0x145)) = 0;
  return 1;
}
