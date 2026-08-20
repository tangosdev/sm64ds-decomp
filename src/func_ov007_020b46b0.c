extern int data_ov007_020d77dc[];
extern char *data_ov007_0210342c;
extern int func_ov007_020b79e4(void);
extern int func_ov007_020c1da0(int i);
extern void func_ov007_020b63e4(void *t);
extern void *func_ov007_020b8f78(void);
extern int func_ov007_020b8fa0(void);
extern void func_ov007_020c1d78(int i);
#pragma opt_propagation off
void func_ov007_020b46b0(void *t, char *p2)
{
  int hit = 0;
  int s = 0;
  unsigned short id;
  {
    char *p = *((char **) t);
    int call_it = hit;
    int t0 = hit;
    int b6 = hit;
    if ((*(*((short **) (p + 4)))) == 0)
    {
      if (((*((int *) (p + 0x10))) >= 0x1000) || ((*((int *) (((char *) data_ov007_020d77dc) + ((*(*((unsigned short **) t))) * 12)))) & 2))
      {
        b6 = 1;
      }
    }
    if (b6)
    {
      if (!((*((int *) (((char *) data_ov007_020d77dc) + ((*(*((unsigned short **) t))) * 12)))) & 1))
      {
        t0 = 1;
      }
    }
    if (t0)
    {
      int ok = 1;
      if (func_ov007_020b79e4())
      {
        int special = 0;
        if (func_ov007_020b79e4())
        {
          unsigned short id2 = *(*((unsigned short **) t));
          int x = ok;
          if ((id2 != 0x16) && (id2 != 0x17))
          {
            x = special;
          }
          if (x)
          {
            special = 1;
          }
        }
        if (!special)
        {
          ok = 0;
        }
      }
      if (ok)
      {
        call_it = 1;
      }
    }
    if (call_it != 0)
    {
      if (func_ov007_020c1da0(0) == 0)
      {
        s = *((unsigned short *) (p2 + 0xc));
        int v14 = 0;
        int v15 = v14;
        if (s != 0)
        {
          if ((*((unsigned short *) (p2 + 0x14))) == 0)
          {
            v15 = 1;
          }
        }
        if (v15)
        {
          if ((*((unsigned int *) (p2 + 0x24))) >= 1)
          {
            v14 = 1;
          }
        }
        if (v14)
        {
          int *pos = *(((int **) t) + 1);
          int dy = pos[2] >> 12;
          int dx = pos[1] >> 12;
          int maxY = (*((int **) ((*((char **) t)) + 0x28)))[4] + dy;
          int minY = (*((int **) ((*((char **) t)) + 0x28)))[2] + dy;
          int maxX = (*((int **) ((*((char **) t)) + 0x28)))[3] + dx;
          int minX = (*((int **) ((*((char **) t)) + 0x28)))[1] + dx;
          if (((((s != 0) && (((int) (*((unsigned short *) (p2 + 8)))) >= minX)) && (((int) (*((unsigned short *) (p2 + 8)))) <= maxX)) && (((int) (*((unsigned short *) (p2 + 0xa)))) >= minY)) && (((int) (*((unsigned short *) (p2 + 0xa)))) <= maxY))
          {
            func_ov007_020b63e4(t);
            goto tail;
          }
          id = *((unsigned short *) (*((char **) t)));
          if (id == 0xc)
          {
            if ((*((unsigned char *) ((*(*((int **) (data_ov007_0210342c + 0x28)))) + 9))) == 0)
            {
              goto rect_c;
            }
          }
          if (id != 0xd)
          {
            goto check_9;
          }
          if ((*((unsigned char *) ((*(*((int **) (data_ov007_0210342c + 0x28)))) + 9))) == 0)
          {
            goto check_9;
          }
          rect_c:
          if (((((s != 0) && (((int) (*((unsigned short *) (p2 + 8)))) >= 8)) && (((int) (*((unsigned short *) (p2 + 8)))) <= 0x66)) && (((int) (*((unsigned short *) (p2 + 0xa)))) >= 0x50)) && (((int) (*((unsigned short *) (p2 + 0xa)))) <= 0x60))
          {
            func_ov007_020b63e4(t);
          }

          goto tail;
          check_9:
          if (id == 9)
          {
            if (((((s != 0) && (((int) (*((unsigned short *) (p2 + 8)))) >= 8)) && (((int) (*((unsigned short *) (p2 + 8)))) <= 0x48)) && (((int) (*((unsigned short *) (p2 + 0xa)))) >= 0x20)) && (((int) (*((unsigned short *) (p2 + 0xa)))) <= 0x30))
            {
              func_ov007_020b63e4(t);
            }
          }

          goto tail;
        }
      }
    }
  }
  if ((*(*((short **) (data_ov007_0210342c + 4)))) == 3)
  {
    {
      volatile char **vt = (volatile char **) t;
      if (((*((unsigned short *) (*vt))) == 3) || ((*((unsigned short *) (*vt))) == 6))
      {
        char *q = *((char **) t);
        int call_it2 = 0;
        int t02 = call_it2;
        int b62 = call_it2;
        if ((*(*((short **) (q + 4)))) == 0)
        {
          if (((*((int *) ((*((char **) t)) + 0x10))) >= 0x1000) || ((*((int *) (((char *) data_ov007_020d77dc) + ((*((unsigned short *) (*((char **) t)))) * 12)))) & 2))
          {
            b62 = 1;
          }
        }
        if (b62)
        {
          if (!((*((int *) (((char *) data_ov007_020d77dc) + ((*((unsigned short *) (*((char **) t)))) * 12)))) & 1))
          {
            t02 = 1;
          }
        }
        if (t02)
        {
          int ok = 1;
          if (func_ov007_020b79e4())
          {
            int special = 0;
            if (func_ov007_020b79e4())
            {
              unsigned short id2 = *(*((unsigned short **) t));
              int x = ok;
              if ((id2 != 0x16) && (id2 != 0x17))
              {
                x = special;
              }
              if (x)
              {
                special = 1;
              }
            }
            if (!special)
            {
              ok = 0;
            }
          }
          if (ok)
          {
            call_it2 = 1;
          }
        }
        if (call_it2)
        {
          if (func_ov007_020c1da0(0) == 0)
          {
            if ((*((unsigned short *) (p2 + 0xc))) != 0)
            {
              if ((*((short *) func_ov007_020b8f78())) == 5)
              {
                if (func_ov007_020b8fa0() == 0)
                {
                  int *pos = *(((int **) t) + 1);
                  int dy = pos[2] >> 12;
                  int dx = pos[1] >> 12;
                  int maxY = (*((int **) ((*((char **) t)) + 0x28)))[4] + dy;
                  int minY = (*((int **) ((*((char **) t)) + 0x28)))[2] + dy;
                  int maxX = (*((int **) ((*((char **) t)) + 0x28)))[3] + dx;
                  int minX = (*((int **) ((*((char **) t)) + 0x28)))[1] + dx;
                  if ((((((*((unsigned short *) (p2 + 0xc))) != 0) && (((int) (*((unsigned short *) (p2 + 8)))) >= minX)) && (((int) (*((unsigned short *) (p2 + 8)))) <= maxX)) && (((int) (*((unsigned short *) (p2 + 0xa)))) >= minY)) && (((int) (*((unsigned short *) (p2 + 0xa)))) <= maxY))
                  {
                    int *pi = (int *) ((*((int *) ((*((char **) t)) + 4))) + 4);
                    hit = 1;
                    *pi = (*pi) + 1;
                    if ((*((int *) ((*((int *) ((*((char **) t)) + 4))) + 4))) > 0x3a)
                    {
                      func_ov007_020b63e4(t);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  tail:
  if (!hit)
  {
    *((int *) ((*((int *) ((*((char **) t)) + 4))) + 4)) = 0;
  }

  {
    short *st = *((short **) ((*((char **) t)) + 4));
    if ((*st) == 2)
    {
      if (st[1] != (-1))
      {
        func_ov007_020c1d78(0);
      }
    }
  }
}
