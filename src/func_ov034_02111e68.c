void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned);
int _ZN9Animation8FinishedEv(void *);
int _ZN6Player18HasFinishedTalkingEv(void *);
void func_ov034_021125b8(char *, int);
extern void **data_ov034_02113888[];
extern void **data_ov034_02113874[];
extern char *data_0209f318;

void func_ov034_02111e68(char *c)
{
  int i;
  int k;
  char *anim;
  unsigned st;
  void *player;

  anim = c + 0x110;
  anim = anim + 0xc8;
  for (i = 2, k = 6; i < 5; i++, k += 3, anim += 0x64)
  {
    if ((*((unsigned char *) (c + 0x8da))) == (k - 3))
    {
      _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_02113888[i][1], 0x40000000, 0x1000, 0);
    }
  }

  {
    int idx;
    char *fin;
    char *an;
    fin = c + 0x160;
    idx = 0;
    an = c;
    an = an + 0x110;
    for (; idx < 5; idx++, fin += 0x64, an += 0x64)
    {
      if (_ZN9Animation8FinishedEv(fin))
      {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(an, data_ov034_02113874[idx][1], 0, 0x1000, 0);
      }
    }
  }

  player = *(void **)(c + 0x8c8);
  {
    int flag;
    int i;
    char *p;
    int zero;
    p = c;
    flag = 1;
    zero = 0;
    i = 0;
    for (; i < 5; i++, p += 0x64)
    {
      if ((*((int *) (p + 0x170))) != ((int) data_ov034_02113874[i][1]))
      {
        flag = zero;
      }
    }

    if (flag == 1)
    {
      if (_ZN6Player18HasFinishedTalkingEv(player))
      {
        st = *((unsigned char *) (c + 0x8db));
        if (st < 4)
        {
          if (st > 1)
          {
            int t = (int) (st - 2);
            int m = t * 0xa;
            *((int *) (c + 0xa8)) = 0x41000 - (m << 0xc);
            *((int *) (c + 0x9c)) = -0x4000;
          }
        }
        (*(int *)(((int)data_0209f318 + 0x154) & 0xFFFFFFFFFFFFFFFF)) &= ~8;
        *((unsigned char *) (c + 0x8e1)) = 0;
        func_ov034_021125b8(c, 4);
      }
    }
  }
  {
    unsigned char *t = (unsigned char *) ((void *) ((unsigned long long) ((unsigned) (c + 0x8da))));
    *t = (unsigned char) ((*t) + 1);
  }
  if ((*((unsigned char *) (c + 0x8da))) > 0xc)
  {
    *((unsigned char *) (c + 0x8da)) = 0xd;
  }
}
