//cpp
extern "C" void func_ov007_020c92a0(int);
extern "C" void func_ov007_020c3d1c(int);
extern "C" void func_ov007_020bc53c(int r0){
  int* r4 = (int*)r0;
  if (r4[0x18/4]) { func_ov007_020c92a0(r4[0x18/4]); r4[0x18/4] = 0; }
  if (r4[0x24/4]) { func_ov007_020c92a0(r4[0x24/4]); r4[0x24/4] = 0; }
  if (r4[0x30/4]) { func_ov007_020c92a0(r4[0x30/4]); r4[0x30/4] = 0; }
  if (r4[0x88/4]) { func_ov007_020c92a0(r4[0x88/4]); r4[0x88/4] = 0; }
  if (r4[0xc/4]) { func_ov007_020c3d1c(r4[0xc/4]); r4[0xc/4] = 0; }
  if (r4[8/4]) { func_ov007_020c3d1c(r4[8/4]); r4[8/4] = 0; }
  func_ov007_020c3d1c(r4[0x84/4]);
  func_ov007_020c3d1c(r4[4/4]);
  func_ov007_020c3d1c(r4[0]);
  func_ov007_020c3d1c((int)r4);
}
