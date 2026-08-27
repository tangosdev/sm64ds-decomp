extern void func_02013f4c(void);
void func_02013f28(void) {
    *(volatile unsigned short*)0x4000208;
    *(volatile unsigned short*)0x4000208 = 0;
    for (;;) func_02013f4c();
}
