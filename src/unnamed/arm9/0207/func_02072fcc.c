
#pragma opt_loop_invariants off
int func_02072fcc(unsigned char *a, unsigned char *b, int *out)
{
  unsigned char *p = b;
  unsigned char *q;
  unsigned char ca;
  unsigned char cb;
  int num;
  unsigned char *new_var;
  *out = 0;
  if (b == 0)
  {
    return 1;
  }
  if ((*b) == 'P')
  {
    if ((*(++p)) == 'V')
    {
      p++;
    }
    if ((*p) == 'K')
    {
      p++;
    }
    if ((*p) == 'v')
    {
      if (((*a) == 'P') || ((*a) == '*'))
      {
        return 1;
      }
    }
    p = b;
  }
  ca = *((volatile unsigned char *) a);
  if ((ca != '!') && (ca != '*'))
  {
    goto tail;
  }
  ca = *a;
  cb = *(p++);
  q = a + 1;
  if (ca != cb)
  {
    return 0;
  }
  a = q;
  loop:
  ca = *a;

  cb = *(p++);
  if (ca != cb)
  {
    goto retry;
  }
  a++;
  new_var = b;
  if (ca != '!')
  {
    goto loop;
  }
  num = 0;
  if ((*((volatile unsigned char *) a)) != '!')
  {
    do
    {
      num = ((num * 10) + (*a)) - '0';
      a++;
    }
    while ((*a) != '!');
  }
  *out = num;
  return 1;
  retry:
  do
  {
    ca = *(a++);
  }
  while (ca != '!');

  do
  {
    ca = *(a++);
  }
  while (ca != '!');
  if ((*a) == 0)
  {
    return 0;
  }
  p = new_var + 1;
  goto loop;
  prblock:
  cb = *(++p);

  a++;
  if (cb == 'K')
  {
    p++;
    if ((*a) == 'K')
    {
      a++;
    }
  }
  ca = *a;
  if (ca == 'K')
  {
    return 0;
  }
  if ((*p) == 'V')
  {
    if (ca == 'V')
    {
      a++;
    }
    p++;
  }
  if ((*((volatile unsigned char *) a)) == 'V')
  {
    return 0;
  }
  tail:
  ca = *a;

  if (((ca == 'P') || (ca == 'R')) && (ca == (*p)))
  {
    goto prblock;
  }
  cb = *p;
  if (ca == cb)
  {
    do
    {
      if (ca == 0)
      {
        return 1;
      }
      ca = *(++a);
      cb = *(++p);
    }
    while (ca == cb);
  }
  return 0;
}
