
typedef unsigned int u32;
extern char data_020a8180[];
extern char data_020a8780[];
extern void func_02060d98(u32 a, u32 b);
typedef struct 
{
  u32 a;
  u32 b;
  u32 c;
} DmaCh;
void func_02060d1c(void)
{
  DmaCh *new_var;
  char *g = data_020a8180;
  DmaCh *ch = (DmaCh *) ((0x40000b0 + ((*((u32 *) (g + 0x24))) * 0xc)) & 0xFFFFFFFFFFFFFFFFULL);
  u32 v;
  new_var = ch;
  new_var->a = 0x4100010;
  new_var->b = *((u32 *) (g + 0x1c));
  new_var->c = 0xaf000001;
  if (((!new_var) && (!new_var)) && (!new_var))
  {
  }
  v = *((u32 *) (g + 0x18));
  func_02060d98((v >> 8) | 0xb7000000, v << 0x18);
  *((u32 *) 0x40001a4) = *((u32 *) (data_020a8780 + 4));
}
