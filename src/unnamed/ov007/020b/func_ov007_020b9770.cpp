//cpp
extern "C" void func_ov007_020c6684(int c);
extern "C" void func_ov007_020c937c(int);
extern "C" void func_ov007_020c9080(int);
extern "C" int func_ov007_020c44c4(int r4);

struct Player { void St_EndingFly_Main(); };

extern int *data_ov007_02104ba0;
extern int *data_ov007_02104b9c;

extern "C" void func_ov007_020b9770(void)
{
  int i;

  if (data_ov007_02104ba0 == 0)
    return;

  func_ov007_020c6684(data_ov007_02104ba0[0]);
  func_ov007_020c937c(data_ov007_02104ba0[1]);
  func_ov007_020c937c(data_ov007_02104ba0[2]);
  func_ov007_020c937c(data_ov007_02104ba0[3]);
  func_ov007_020c937c(data_ov007_02104ba0[4]);
  func_ov007_020c9080(data_ov007_02104ba0[0x11]);
  func_ov007_020c9080(data_ov007_02104ba0[0x12]);

  for (i = 0; i < 2; i++)
    func_ov007_020c44c4((data_ov007_02104b9c + i)[1]);

  func_ov007_020c44c4(data_ov007_02104b9c[3]);
  func_ov007_020c44c4(data_ov007_02104b9c[4]);
  ((Player *)data_ov007_02104ba0)->St_EndingFly_Main();
  data_ov007_02104ba0 = 0;
  ((Player *)data_ov007_02104b9c)->St_EndingFly_Main();
  data_ov007_02104b9c = 0;
}
