
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned char u8;
void func_ov006_0211d018(char *base, int idx)
{
  int off = idx * 0x1c;
  int new_var2;
  char *new_var;
  char *r;
  if ((*((u8 *) (((base + off) + 0x4000) + 0xbe5))) != 0)
  {
    *((u8 *) (((base + off) + 0x4000) + 0xbe1)) = 6;
    if (1)
    {
      return;
    }
  }
  char *r3 = (base + 0x4bde) + off;
  if ((*((u16 *) r3)) != 0)
  {
    *((u16 *) r3) = (*((u16 *) r3)) - 1;
    return;
  }
  *((u8 *) (((base + off) + 0x4000) + 0xbe2)) = 1;
  new_var2 = 1;
  ;
  char *r4 = (base + 0x4bcc) + off;
  if (((*((s32 *) r4)) >> 0xc) < 0)
  {
    *((u8 *) (((base + off) + 0x4000) + 0xbe4)) = new_var2;
    *((s32 *) r4) = -0x80000;
    *((s32 *) (((base + off) + 0x4000) + 0xbd4)) = 0x4800;
    *((u8 *) (((base + off) + 0x4000) + 0xbe1)) = 3;
    return;
  }
  *((u8 *) (((base + off) + 0x4000) + 0xbe4)) = 0;
  *((s32 *) r4) = 0x180000;
  *((s32 *) (((base + off) + 0x4000) + 0xbd4)) = -0x4800;
  *((u8 *) (((base + off) + 0x4000) + 0xbe1)) = 2;
}
