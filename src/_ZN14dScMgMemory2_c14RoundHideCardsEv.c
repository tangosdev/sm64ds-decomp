
void _ZN14dScMgMemory2_c14RoundHideCardsEv(char *o)
{
  int count;
  int i;
  unsigned char *e;
  if ((*((unsigned short *) (o + 0x53ec))) != 0)
  {
    i = 0x53ec;
    *((unsigned short *) ((((int) o) + i))) -= 1;
    return;
  }
  e = (unsigned char *) o;
  for (i = 0; i < 0x14; i++)
  {
    if (e[0x51bb] != 0)
    {
      if (e[0x51bc] == 4)
      {
        e[0x51bc] = 5;
      }
    }
    e += 0x18;
  }

  count = 0;
  i = 0;
  for (e = (unsigned char *) o; i < 0x14; i++)
  {
    if (e[0x51bb] != 0)
    {
      unsigned char st = e[0x51bc];
      if ((st != 1) && (st != 5))
      {
        count++;
        break;
      }
    }
    e = e + 0x18;
  }

  if (count == 0)
  {
    *((int *) (o + 0x53d8)) = 7;
  }
}
