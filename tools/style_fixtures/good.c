typedef unsigned int u32;

void SetFixtureRegister(void) {
    *(volatile u32 *)0x04000208 = 0;
}
