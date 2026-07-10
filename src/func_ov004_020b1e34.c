/* func_ov004_020b1e34 @ 0x20b1e34 (ov004) -- veneer: ldr r1,[r0,#0xb4]; b func_ov004_020b0e84. */
typedef unsigned int u32;
extern void func_ov004_020b0e84(void*, u32);

void func_ov004_020b1e34(void* a) {
    func_ov004_020b0e84(a, *(u32*)((char*)a + 0xb4));
}
