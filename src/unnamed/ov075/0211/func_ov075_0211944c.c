extern int func_0203d974(void);
extern int LoadFile(int id);
extern void func_ov075_02116030(void* c, int v);
extern void func_020338b0(int a, int b, int c, int d);
extern unsigned char data_0209d454;
extern int data_ov075_0211d71c;
void func_ov075_0211944c(char* c) {
  if (func_0203d974()) {
    int f;
    data_0209d454 &= ~5;
    f = LoadFile(0x8409);
    func_ov075_02116030(&data_ov075_0211d71c, f);
    func_020338b0(0x225, 0xb0, 0x1d, -1);
    data_0209d454 |= 5;
    *(unsigned char*)(c + 0x280) = 0;
    *(int*)(c + 0x26c) = 0;
    *(int*)(c + 0x270) = 0x78;
  }
  *(unsigned char*)(c + 0x282) = 0;
  *(int*)(c + 0x268) = 0;
  *(int*)(c + 0x274) = 0;
}
