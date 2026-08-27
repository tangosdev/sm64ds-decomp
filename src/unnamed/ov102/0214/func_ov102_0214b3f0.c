extern void func_ov102_0214bd20(void *c);
void func_ov102_0214b3f0(void *c, int a1) {
    unsigned int v = *(int*)((char*)c + 0x3dc) - 3;
    if (v <= 1u) return;
    unsigned short h = *(unsigned short*)((char*)c + 0x3ea);
    if (h > 4u) goto do_call;
    if (h != 0u) return;
do_call:
    *(int*)((char*)c + 0x390) = a1;
    func_ov102_0214bd20(c);
}
