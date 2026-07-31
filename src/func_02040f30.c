
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef struct 
{
  u32 kind : 4;
  u32 chan : 4;
  u32 flag : 24;
  u32 word4;
  u8 byte8;
} Cmd;
extern int func_02065a4c(int x);
extern void func_02065788(u16 type, u32 arg1, const void *src, u32 arg3);
extern void func_02041a00(int x);
extern void func_02065730(u16 type, u32 arg1, const void *src1, u32 arg3, const void *src2, u32 extra);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern int func_020423dc(int *r6);
extern int func_020656d4(u16 arg0, const void *arg1, int arg2, u32 arg3);
extern void func_020657d0(u16 id, void *obj);
extern void func_0205cd5c(void *self, int arg);
extern void func_02042160(void);
extern void func_02040f30(void *arg0);
extern char data_020a1fc0[];
extern char data_020a2400[];
void func_02040f30(void *arg0)
{
  char *thiz = (char *) arg0;
  char *g2400;
  int idx;
  int type;
  idx = func_02065a4c(*((u16 *) (thiz + 0xa)));
  type = *((int *) thiz);
  g2400 = data_020a2400;
  switch (type)
  {
    case 2:
      if ((*((u16 *) (thiz + 8))) != 0)
    {
      return;
    }
      *((int *) (data_020a1fc0 + 0x28)) = (*((short *) (thiz + 0x16))) + 0xe;
      *((int *) (data_020a1fc0 + 0x2c)) = (*((short *) (thiz + 0x18))) + 0xe;
      if ((*((int *) (data_020a1fc0 + 0xc))) != 2)
    {
      func_02065788((u16) 1, 0, (const void *) g2400, (u32) func_02040f30);
      return;
    }
    {
      int f1c = *((int *) (data_020a1fc0 + 0x1c));
      if (f1c != 0)
      {
        func_02041a00(f1c);
      }
    }
      return;

    case 6:
      *((u32 *) ((g2400 + (idx * 4)) + 0x40)) = *((u32 *) (data_020a1fc0 + 0x20));
      func_02065730((u16) 1, 0x20000, (const void *) (g2400 + 0x40), *((u32 *) (data_020a1fc0 + 0x24)), (const void *) (g2400 + 0x80), (u32) func_02040f30);
      return;

    case 4:
      if ((*((u16 *) (thiz + 8))) != 0)
    {
      return;
    }
      if (((*((int *) (data_020a1fc0 + 0xc))) != 2) != 0)
    {
      _ZN4CP1527FlushAndInvalidateDataCacheEjj(*((u32 *) (data_020a1fc0 + 0x20)), *((u32 *) (data_020a1fc0 + 0x24)));
      func_020423dc((int *) (*((u32 *) (data_020a1fc0 + 0x20))));
      {
        *((int *) (data_020a1fc0 + 0xc)) = 2;
        {
          int fn = *((int *) (data_020a1fc0 + 0x10));
          if (fn != 0)
          {
            ((void (*)(int)) fn)(0);
          }
        }
      }
      return;
    }
    {
      u32 w4;
      int c;
      w4 = *((u32 *) (g2400 + 0xc78));
      Cmd cmd;
      cmd.kind = 2;
      cmd.flag = 1;
      c = *((int *) (data_020a1fc0 + 0x28));
      cmd.word4 = w4;
      cmd.chan = (u8) (c >> 8);
      cmd.byte8 = (u8) c;
      func_020656d4((u16) 1, &cmd, 9, (u32) func_02042160);
    }
      return;

    case 0xc:
    {
      Cmd *m;
      if ((*((int *) (thiz + 4))) != 0xa)
      {
        return;
      }
      m = (Cmd *) ((((int) thiz) + 0x14));
      switch (m->kind)
      {
        case 1:
          if (m->flag == 0)
        {
          func_020657d0((u16) 1, (void *) func_02040f30);
          return;
        }
          *((u32 *) (g2400 + 0xc78)) = m->word4;
          *((u32 *) ((g2400 + (idx * 4)) + 0x40)) = *((u32 *) ((*((char **) (data_020a1fc0 + 0x1c))) + 0x2c));
          func_02065730((u16) 1, *((u32 *) (g2400 + 0xc78)), (const void *) (g2400 + 0x40), *((u32 *) ((*((char **) (data_020a1fc0 + 0x1c))) + 0x34)), (const void *) (g2400 + 0x80), (u32) func_02040f30);
          return;

        case 3:
        {
          void *p = *((void **) (data_020a1fc0 + 0x1c));
          void *a0 = *((void **) (((char *) p) + 8));
          *((int *) (data_020a1fc0 + 0x1c)) = 0;
          *((int *) ((((int) p) + 0x28))) += *((int *) (((char *) p) + 0x34));
          func_0205cd5c(a0, 0);
          return;
        }

      }

      return;
    }

  }

}
