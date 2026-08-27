extern void _ZN3IRQ11DisableIRQsEj(unsigned int);
extern int func_02053c10(int);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned int, void(*)(void));
extern unsigned char data_0209d454;
void func_ov006_020eff20(char* c, int idx){
  volatile unsigned short* ime = (volatile unsigned short*)0x4000208;
  unsigned short saved;
  *(unsigned char*)(c + idx * 0x14 + 0x47f4) = 0;
  saved = *ime;
  *ime = 0;
  _ZN3IRQ11DisableIRQsEj(2);
  func_02053c10(0);
  _ZN3IRQ13SetIRQHandlerEjPFvvE(2, 0);
  if (saved != 0) { *ime; *ime = 1; }
  *(volatile unsigned int*)0x4000000 &= ~0xe000;
  *(volatile unsigned int*)0x4001000 &= ~0xe000;
  data_0209d454 |= 4;
}
