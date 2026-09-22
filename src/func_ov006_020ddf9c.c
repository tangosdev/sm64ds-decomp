typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;
void func_ov006_020dc7fc(char *p);
void func_ov006_020dc334(char *p);
void func_ov006_020ddf9c(char *c)
{
  int i, k, j, i2, i3, i4;
  char *p = c;
  k = 0;
  for (; k < 0x28; k++)
  {
    i = 0;
    *((u32 *) (p + 0x4660)) = i;
    *((u32 *) (p + 0x4664)) = i;
    *((u32 *) (p + 0x4668)) = i;
    *((u32 *) (p + 0x466c)) = i;
    *((u16 *) (p + 0x4670)) = i;
    *((u16 *) (p + 0x4672)) = i;
    *((u8 *) (p + 0x4674)) = i;
    *((u8 *) (p + 0x4675)) = i;
    *((u8 *) (p + 0x4676)) = i;
    *((u8 *) (p + 0x4677)) = i;
    *((u8 *) (p + 0x4678)) = i;
    *((u8 *) (p + 0x4679)) = i;
    *((u8 *) (p + 0x467a)) = i;
    *((u8 *) (p + 0x467b)) = i;
    p += 0x1c;
  }

  p = c;
  for (i2 = 0; i2 < 0x18; i2++)
  {
    j = 0;
    *((u32 *) (p + 0x4ac0)) = j;
    *((u32 *) (p + 0x4ac4)) = j;
    *((u8 *) (p + 0x4ad0)) = j;
    *((u8 *) (p + 0x4ad1)) = j;
    *((u8 *) (p + 0x4ad2)) = j;
    *((u8 *) (p + 0x4ad3)) = j;
    *((u8 *) (p + 0x4ad4)) = j;
    *((u8 *) (p + 0x4ad5)) = j;
    p += 0x18;
  }

  *((u32 *) (c + 0x4d00)) = 0;
  *((u32 *) (c + 0x4d04)) = 0;
  *((u16 *) (c + 0x4d08)) = 0;
  *((u16 *) (c + 0x4d0a)) = 0;
  *((u16 *) (c + 0x4d0c)) = 0;
  *((u8 *) (c + 0x4d13)) = 0;
  p = c;
  for (i3 = 0; i3 < 0x20; i3++)
  {
    *((u8 *) (p + 0x4d28)) = 0;
    *((u8 *) (p + 0x4d29)) = 0;
    p += 0x18;
  }

  for (i4 = 0; i4 < 0x18; i4++)
  {
    char *e = c + ((i4 & 0xFFFFFFFFu) << 4);
    *((u8 *) (e + 0x5020)) = 0;
    *((u8 *) (e + 0x5021)) = 0;
  }

  *((u32 *) (c + 0x51c8)) = 0;
  *((u32 *) (c + 0x51cc)) = 0;
  *((u8 *) (c + 0x51db)) = 0;
  *((u8 *) (c + 0x51dd)) = 0;
  *((u32 *) (c + 0x51d0)) = 0;
  *((u8 *) (c + 0x51de)) = 0;
  func_ov006_020dc7fc(c);
  func_ov006_020dc334(c);
  *((u8 *) (c + 0x51df)) = 0;
}
