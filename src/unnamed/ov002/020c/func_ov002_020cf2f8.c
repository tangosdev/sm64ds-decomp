extern int _Z14ApproachLinearRiii(void*, int, int);
extern int ApproachAngle(void*, int, int, int, int);
extern int func_ov002_020bf224(void*, int, int);
extern unsigned char data_020a0e40[];
extern short data_0209f4a0[];
void func_ov002_020cf2f8(void* c) {
  if (*(short*)((char*)data_0209f4a0 + data_020a0e40[0]*0x18) == 0) return;
  _Z14ApproachLinearRiii((char*)c+0x98, func_ov002_020bf224(c, 0x8000, 0x2000), 0x1000);
  ApproachAngle((char*)c+0x94, *(short*)((char*)c+0x6d2), 0x10, 0x1000, 0x200);
  *(short*)((char*)c+0x8e) = *(short*)((char*)c+0x94);
}
