extern int func_0203da9c(void);
extern int GetPlayerFlagByte(int a, int b);
extern void func_0203db3c(int a, unsigned char b);
void func_020200e0(void){
  int x;
  if (func_0203da9c() != 0) return;
  x = GetPlayerFlagByte(0, 2);
  func_0203db3c(2, (unsigned char)(x & ~0x20));
}
