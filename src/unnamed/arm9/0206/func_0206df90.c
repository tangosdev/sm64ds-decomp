
void func_0206e068(int a, int *b);
void func_0206e030(void *p);
typedef int (*Fn)(int, int, int *, int);
int func_0206df90(char *thiz, int *out)
{
  char *new_var;
  int diff = (*((int *) (thiz + 0x24))) - (*((int *) (thiz + 0x1c)));
  if (diff != 0)
  {
    int r;
    *((int *) (thiz + 0x28)) = diff;
    if (((((unsigned int) ((*((unsigned int *) (thiz + 4))) << 0x13)) >> 9) >> 22) == 0)
    {
      func_0206e068(*((int *) (thiz + 0x1c)), (int *) (thiz + 0x28));
    }
    r = (*((Fn *) (thiz + 0x40)))(*((int *) thiz), *((int *) (thiz + 0x1c)), (int *) (thiz + 0x28), *((int *) (thiz + 0x48)));
    if (out != 0)
    {
      *out = *((int *) (thiz + 0x28));
    }
    if (r != 0)
    {
      return r;
    }
    new_var = thiz + 0x18;
    *(int *)(new_var) += *((int *) (thiz + 0x28));
  }
  func_0206e030(thiz);
  return 0;
}
