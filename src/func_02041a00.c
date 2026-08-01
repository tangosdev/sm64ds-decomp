#include "types.h"
extern char data_020a2400[];
extern char data_020a1fc0[];
extern void func_02041224(char *thiz, int n);
extern int func_020656d4(u16 arg0, const void *arg1, s32 arg2, u32 arg3);
extern void func_02042160(void);
typedef struct 
{
  u32 f0 : 4;
  u32 f4 : 4;
  u32 f8 : 24;
  u32 w1;
  u8 b;
} Msg;
void func_02041a00(char *thiz)
{
  Msg m;
  int g28;
  int f34;
  int f28;
  func_02041224(data_020a2400, *((int *) (thiz + 0x34)));
  *((int *) (data_020a1fc0 + 0x1c)) = (int) thiz;
  f34 = *((int *) (thiz + 0x34));
  f28 = *((int *) (thiz + 0x28));
  m.f0 = 0;
  m.f8 = f34;
  g28 = *((int *) (data_020a1fc0 + 0x28));
  m.w1 = f28;
  m.f4 = (g28 >> 8) & 0xff;
  m.b = (u8) g28;
  func_020656d4(1, &m, 9, (u32) func_02042160);
}
