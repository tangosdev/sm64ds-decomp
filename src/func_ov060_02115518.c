
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
extern int _ZN6Player9StartTalkER9ActorBaseb(void *player, void *actor, int flag);
extern int _ZN6Player12GetTalkStateEv(void *player);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *player, void *actor, unsigned int msg, void *pos, unsigned int a, unsigned int b);
extern void func_02012694(int a, void *v);
extern int func_ov060_02111f08(void *self);
extern s16 data_02082214[];
extern s16 data_ov060_0211a4e0[];
void func_ov060_02115518(char *self)
{
  void *player = *((void **) (self + 0x3a0));
  u8 *new_var;
  if (player == 0)
  {
    return;
  }
  switch (*((u8 *) (self + 0x424)))
  {
    case 0:
      if (_ZN6Player9StartTalkER9ActorBaseb(player, self, 1) == 0)
    {
      return;
    }
      *((int *) (self + 0x40c)) = 6;
    {
      u8 *p = (u8 *) ((void *) (((long long) ((int) (self + 0x424)))));
      *p = (*p) + 1;
    }
      return;

    case 1:
      if (_ZN6Player12GetTalkStateEv(player) != 0)
    {
      return;
    }
    {
      int pos[3];
      int x;
      int y;
      int z;
      int y2;
      int zero;
      int scale;
      int msg;
      s16 *tbl;
      s16 *msgs;
      u16 ang;
      s16 sx;
      s16 sz;
      x = *((int *) (self + 0x5c));
      tbl = data_02082214;
      pos[0] = x;
      y = *((int *) (self + 0x60));
      scale = 0xc8;
      pos[1] = (scale) ? (y) : (y);
      z = *((int *) (self + 0x64));
      y2 = y + 0xc8000;
      pos[2] = z;
      tbl = (s16 *) ((void *) data_02082214);
      ang = *((u16 *) (self + 0x8e));
      zero = 0;
      msgs = data_ov060_0211a4e0;
      sx = (s16) tbl[(ang >> 4) * 2];
      pos[0] = (((s16) sx) * ((s16) scale)) + x;
      tbl = data_02082214;
      ang = *((u16 *) (self + 0x8e));
      sz = (s16) tbl[((ang >> 4) * 2) + 1];
      pos[1] = y2;
      pos[2] = (((s16) sz) * ((s16) scale)) + z;
      msg = (s16) msgs[*((u8 *) (self + 0x414))];
      if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*((void **) (self + 0x3a0)), self, msg, pos, zero, zero) == 0)
      {
        return;
      }
    }
    {
      u8 *p = (u8 *) ((void *) (((long long) ((int) (self + 0x424)))));
      *p = (*p) + 1;
    }
      func_02012694(0xb7, self + 0x74);
      return;

    case 2:
      if (_ZN6Player12GetTalkStateEv(player) != (-1))
    {
      return;
    }
      if ((*((u8 *) (self + 0x414))) == 1)
    {
      *((int *) (self + 0x40c)) = 0xd;
    }
    else
    {
      *((int *) (self + 0x40c)) = 0;
    }
    {
      u8 *p = (u8 *) ((void *) (((long long) ((int) (self + 0x424)))));
      *p = (*p) + 1;
    }
      new_var = (u8 *) (self + 0x444);
      *new_var = 4;
      func_ov060_02111f08(self);
      return;

  }

}
