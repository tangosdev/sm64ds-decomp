
void func_ov006_020def80(char *c, int i)
{
  unsigned char t;
  char *b = c + i;
  int new_var;
  b += 0x5000;
  t = *((unsigned char *) (b + 0x465));
  if (t == 0)
  {
    t = *((unsigned char *) (b + 0x462));
    if (t != 0)
    {
      char *s = c + (i << 2);
      s += 0x5000;
      *((int *) (s + 0x434)) = 4;
    }
    else
    {
      char *s = c + (i << 2);
      s += 0x5000;
      *((int *) (s + 0x434)) = 1;
    }
    goto epilogue;
  }
  t = *((unsigned char *) (b + 0x462));
  if (t != 0)
  {
    char *s = c + (i << 2);
    s += 0x5000;
    *((int *) (s + 0x434)) = 5;
    {
      char *p;
      if ((*((unsigned char *) ((c + 0x5000) + 0x46d))) == 0xff)
      {
        if ((*((unsigned char *) ((c + 0x5000) + 0x469))) != 1)
        {
          *((unsigned char *) ((c + 0x5000) + 0x46d)) = (unsigned char) i;
        }
      }
    }
    goto epilogue;
  }
  {
    char *s = c + (i << 2);
    s += 0x5000;
    *((int *) (s + 0x434)) = 2;
  }
  new_var = i;
  epilogue:
  {
    char *s = c + (new_var << 2);
    s += 0x5000;
    *((int *) (s + 0x440)) = 0;
    *((int *) (s + 0x44c)) = 0;
  }

}
