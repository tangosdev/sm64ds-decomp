
typedef int Fix12i;
extern void func_ov002_020c2f64(void *c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *p, void *st);
extern void func_02022b04(Fix12i x, Fix12i y, Fix12i z);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(Fix12i x, Fix12i y, Fix12i z);
extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5);
extern void func_ov002_020bedd4(void *p);
extern unsigned char data_020a0e40;
extern unsigned char data_0209f49e[];
extern char data_ov002_0211013c;
extern char data_ov002_021103dc;
extern char data_ov002_021101b4;
int _ZN6Player17St_WallSlide_MainEv(char *c)
{
  *((int *) (c + 0x684)) = *((int *) (c + 0x60));
  if ((*((unsigned char *) (c + 0x6de))) == 0)
  {
    func_ov002_020c2f64(c);
    _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
  }
  else
  {
    if ((*((unsigned char *) (c + 0x703))) != 0)
    {
      func_02022b04(*((int *) (c + 0x5c)), *((int *) (c + 0x60)), *((int *) (c + 0x64)));
    }
    else
    {
      _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*((int *) (c + 0x5c)), *((int *) (c + 0x60)), *((int *) (c + 0x64)));
    }
    *((int *) (c + 0x620)) = func_0201226c(*((int *) (c + 0x620)), 0, (*((int *) (c + 0x66c))) + 0xe2, (int) (c + 0x74), *((int *) (c + 0x98)), 0);
    if ((*((unsigned short *) (&data_0209f49e[data_020a0e40 * 0x18]))) & 2)
    {
      char *p = c + 0x600;
      if (*((unsigned short *) (p + 0xa4)))
      {
        *((short *) (c + 0x8e)) = *((short *) ((c + 0x600) + 0x9c));
      }
      _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021103dc);
    }
    else
    {
      if ((*((unsigned char *) (c + 0x6e9))) & 2)
      {
        goto end;
      }
      _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
    }
  }
  end:
  func_ov002_020bedd4(c);
  return 1;

}
