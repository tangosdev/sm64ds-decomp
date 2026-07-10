/* func_ov002_020ea410 @ 0x20ea410 (ov002) -- veneer: ldr r1,[r0,#0x438]; b func_ov002_020e8ef0. */
typedef unsigned int u32;
extern void func_ov002_020e8ef0(void*, u32);

void func_ov002_020ea410(void* a) {
    func_ov002_020e8ef0(a, *(u32*)((char*)a + 0x438));
}
